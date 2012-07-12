#!/usr/bin/env python
#
# Copyright 2005,2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gru, eng_notation, optfir, window
from gnuradio import audio
#updated 2011 May 27, MR
from gnuradio import uhd
#from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
#from usrpm import usrp_dbid
import sys
import math
import struct
import time

import os


class tune(gr.feval_dd):
    """
    This class allows C++ code to callback into python.
    """
    def __init__(self, tb):
        gr.feval_dd.__init__(self)
        self.tb = tb

    def eval(self, ignore):
        """
        This method is called from gr.bin_statistics_f when it wants to change
        the center frequency.  This method tunes the front end to the new center
        frequency, and returns the new frequency as its result.
        """
        try:
            # We use this try block so that if something goes wrong from here 
            # down, at least we'll have a prayer of knowing what went wrong.
            # Without this, you get a very mysterious:
            #
            #   terminate called after throwing an instance of 'Swig::DirectorMethodException'
            #   Aborted
            #
            # message on stderr.  Not exactly helpful ;)

            new_freq = self.tb.set_next_freq()
            return new_freq

        except Exception, e:
            print "tune: Exception: ", e


class parse_msg(object):
    def __init__(self, msg):
        self.center_freq = msg.arg1()
        self.vlen = int(msg.arg2())
        assert(msg.length() == self.vlen * gr.sizeof_float)

        # FIXME consider using Numarray or NumPy vector
        t = msg.to_string()
        self.raw_data = t
        self.data = struct.unpack('%df' % (self.vlen,), t)


class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage = "usage: %prog [options] min_freq max_freq"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=("A:0",0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("", "--tune-delay", type="eng_float", default=0.001, metavar="SECS",
                          help="time to delay (in seconds) after changing frequency [default=%default]")
        parser.add_option("", "--dwell-delay", type="eng_float", default=.05, metavar="SECS",
                          help="time to dwell (in seconds) at a given frequncy [default=%default]")
        parser.add_option("-F", "--fft-size", type="int", default=1024,
                          help="specify number of FFT bins [default=%default]")
        #updated 2011 May 27, MR
        parser.add_option("-s", "--samp_rate", type="intx", default=6000000,
        				  help="set sample rate to SAMP_RATE [default=%default]")
        parser.add_option("", "--chan-bandwidth", type="intx", default=6000000,
        				  help="set channel bw [default=%default]")
        #parser.add_option("-d", "--decim", type="intx", default=16,
        #                  help="set decimation to DECIM [default=%default]")
        parser.add_option("", "--real-time", action="store_true", default=False,
                          help="Attempt to enable real-time scheduling")
        #parser.add_option("-B", "--fusb-block-size", type="int", default=0,
        #                  help="specify fast usb block size [default=%default]")
        #parser.add_option("-N", "--fusb-nblocks", type="int", default=0,
        #                  help="specify number of fast usb blocks [default=%default]")
        #options added 2011 May 31, MR
        parser.add_option("", "--threshold", type="eng_float", default=-70, 
        				  help="set detection threshold [default=%default]")
        parser.add_option("", "--num-tests", type="intx", default=200,
        				  help="set the number of times to test the frequency band [default=%default]")
        parser.add_option("", "--log-file", action="store_true", default=False,
                          help="log output to a file")

        (options, args) = parser.parse_args()
        if len(args) != 2:
            parser.print_help()
            sys.exit(1)
            
        self.num_tests = options.num_tests
            
        self.threshold = options.threshold
        self.samp_rate = options.samp_rate
        
        self.min_freq = eng_notation.str_to_num(args[0])
        self.max_freq = eng_notation.str_to_num(args[1])
        
        self.log_file = options.log_file
        
        self.num_channels = int((self.max_freq - self.min_freq)/self.samp_rate) + 2
        
        if self.min_freq > self.max_freq:
            self.min_freq, self.max_freq = self.max_freq, self.min_freq   # swap them
            
        self.fft_size = options.fft_size


        if not options.real_time:
            realtime = False
        else:
            # Attempt to enable realtime scheduling
            r = gr.enable_realtime_scheduling()
            if r == gr.RT_OK:
                realtime = True
            else:
                realtime = False
                print "Note: failed to enable realtime scheduling"

        #removed 2011 May 27, MR
        # If the user hasn't set the fusb_* parameters on the command line,
        # pick some values that will reduce latency.

        #if 1:
        #    if options.fusb_block_size == 0 and options.fusb_nblocks == 0:
        #        if realtime:                        # be more aggressive
        #            options.fusb_block_size = gr.prefs().get_long('fusb', 'rt_block_size', 1024)
        #            options.fusb_nblocks    = gr.prefs().get_long('fusb', 'rt_nblocks', 16)
        #        else:
        #            options.fusb_block_size = gr.prefs().get_long('fusb', 'block_size', 4096)
        #            options.fusb_nblocks    = gr.prefs().get_long('fusb', 'nblocks', 16)
    
        #print "fusb_block_size =", options.fusb_block_size
	    #print "fusb_nblocks    =", options.fusb_nblocks

        # build graph
        
        #updated 2011 May 27, MR
        self.u = uhd.usrp_source(device_addr="", io_type=uhd.io_type.COMPLEX_FLOAT32, num_channels=1)
        self.u.set_subdev_spec("A:0", 0)
        self.u.set_antenna("TX/RX", 0)
        self.u.set_samp_rate(options.samp_rate)
        #self.u = usrp.source_c(fusb_block_size=options.fusb_block_size,
        #                       fusb_nblocks=options.fusb_nblocks)


        #adc_rate = self.u.adc_rate()                # 64 MS/s
        #usrp_decim = options.decim
        #self.u.set_decim_rate(usrp_decim)
        self.usrp_rate = self.u.get_samp_rate() #adc_rate / usrp_decim
        print "sample rate is", self.usrp_rate

        #self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
        #self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        
        """I'm commenting the code below just because is a print. It shows the time of sub-dev that is being used"""
        """print "Using RX d'board %s" % (self.u.get_dboard_sensor_names(chan=0))#(self.subdev.side_and_name(),)"""
        
        s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        c2mag = gr.complex_to_mag_squared(self.fft_size)

        # FIXME the log10 primitive is dog slow
        log = gr.nlog10_ff(10, self.fft_size, -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
		
        # Set the freq_step to 75% of the actual data throughput.
        # This allows us to discard the bins on both ends of the spectrum.

        #changed on 2011 May 31, MR -- maybe change back at some point
        #self.freq_step = 0.75 * self.usrp_rate

	#The steps are ruled by the bandwidth parameter. You may want to change this for something I can't imagine now
        self.freq_step = options.chan_bandwidth 
        self.min_center_freq = self.min_freq + self.freq_step/2
        nsteps = math.ceil((self.max_freq - self.min_freq) / self.freq_step)
        self.max_center_freq = self.min_center_freq + (nsteps * self.freq_step)

        self.next_freq = self.min_center_freq
        
        tune_delay  = max(0, int(round(options.tune_delay * self.usrp_rate / self.fft_size)))  # in fft_frames
        dwell_delay = max(1, int(round(options.dwell_delay * self.usrp_rate / self.fft_size))) # in fft_frames

        self.msgq = gr.msg_queue(16)
        self._tune_callback = tune(self)        # hang on to this to keep it from being GC'd
        
	stats = gr.bin_statistics_f(self.fft_size, self.msgq,
                                    self._tune_callback, tune_delay, dwell_delay)

        # FIXME leave out the log10 until we speed it up
        #self.connect(self.u, s2v, fft, c2mag, log, stats)
        self.connect(self.u, s2v, fft, c2mag, stats)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            # updated 2011 May 31, MR
            #g = self.subdev.gain_range()
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2

        self.set_gain(options.gain)
        print "gain =", options.gain
        
    def set_next_freq(self):
        target_freq = self.next_freq
        self.next_freq = self.next_freq + self.freq_step
        if self.next_freq >= self.max_center_freq:
        	self.next_freq = self.min_center_freq
        	
        if not self.set_freq(target_freq):
        	print "Failed to set frequency to", target_freq
        		
        return target_freq
        	
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.
        	
        @param target_freq: frequency in Hz
        @rypte: bool
        	
        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        #updated 2011 May 31, MR
        #return self.u.tune(0, self.subdev, target_freq)
        return self.u.set_center_freq(target_freq, 0)
        	
    def set_gain(self, gain):
        #updated 2011 May 31, MR
        #self.subdev.set_gain(gain)
        self.u.set_gain(gain)


def main_loop(tb):
	if tb.log_file:
		#filename = "spectrum_sense_exp_" + time.strftime('%y%m%d_%H%M%S') + ".csv"
		#f = open(filename, 'w')
		#f.write("detecting on %s BW channels between " %(tb.samp_rate))
		#f.write("%s and " %(tb.min_freq))
		#f.write("%s\n" %(tb.max_freq))
		#f.close()
		
		#filename for the freq pow file
		filename_p_f = "sensing_" + eng_notation.num_to_str(tb.min_freq) + "_" + eng_notation.num_to_str(tb.max_freq) + time.strftime('%y%m%d_%H%M%S') + ".txt"
		
	i = 0
	mywindow = window.blackmanharris(tb.fft_size)
	power = 0
	for tap in mywindow:
		power += tap*tap
		
	k = -20*math.log10(tb.fft_size)-10*math.log10(power/tb.fft_size)
	
	while i < tb.num_tests or tb.num_tests == 0:
		i = (i+1)
				
		# Get the next message sent from the C++ code (blocking call).
		# It contains the center frequency and the mag squared of the fft
		m = parse_msg(tb.msgq.delete_head())
		
		#fft_sum_db = 20*math.log10(sum(m.data)/m.vlen)
		temp_list = []
		temp_list2 = []
		for item in m.data:
			temp_list.append(10*math.log10(item) + k)
			"""in the documentation it says that m.data carries the power squared"""
			sqr_power = math.sqrt(item)
			temp_list2.append(10*math.log10(sqr_power) + k)
		fft_sum_db = sum(temp_list2)/m.vlen
		avg_power = str(fft_sum_db)
		current_freq = str(m.center_freq)
		binary_file = "binary_floats.dat"  
				
		"""the tb.threshold is the rx threshold"""
		if tb.log_file:
			#f = open(filename, 'a')
			d = open(filename_p_f, 'a')
			"""creating a binary file for appending the m.raw_data"""
			#b = open(binary_file,"ab+")
			#b.write(m.raw_data)
			d.write(current_freq + ' ' + avg_power + '\n')
			#if fft_sum_db > tb.threshold:
			#	f.write("1,")
			#else:
			#	f.write("0,")
			#f.write(str(m.center_freq))
			#f.write(", ")
			#f.write(str(fft_sum_db))
			#f.write("\n")
			#if m.center_freq >= tb.max_center_freq - tb.freq_step:
			#	f.write("\n")
			#	break
			#f.close()
			#b.close()
			d.close()
			
		if not tb.log_file and m.center_freq == tb.min_center_freq:
				os.system("clear")
				#tb.next_freq = tb.min_center_freq
				#tb.msgq.flush()
		
		print m.center_freq, fft_sum_db

		if not tb.log_file and m.center_freq >= tb.max_center_freq - tb.freq_step:
				time.sleep(.5)
				tb.next_freq = tb.min_center_freq
				tb.msgq.flush()

		#if i == (tb.num_tests):
			#os.system("Put the terminal command inside")
		
	#f.close()
	

    
if __name__ == '__main__':
    tb = my_top_block()
    try:
        tb.start()              # start executing flow graph in another thread...
        main_loop(tb)
        tb.stop()
        tb.wait()
        
    except KeyboardInterrupt:
        pass
