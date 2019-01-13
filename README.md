# parallfetch
Fast Application Launch on Personal Computing/Communication Devices

# Install steps
1. Kernel build
   1) Download kernel 4.12.9 (https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/linux-4.12.9.tar.xz)
   2) Applying linux-4.12.9-pf.patch to the downloaded kernel
   3) Check "File systems -> Paralfetch for quick application launches" option before building kernel
3. Build utility <br />
   $ cd parallfetch_tools; make <br />
   $ mv pfsetmode pfviewer /usr/bin <br />
   $  sudo chmod u+s /usr/bin/pfsetmode <br />
4. creating prefetch file directory (for storing <app_name>.pf) <br />
   $ sudo mkdir /flashfetch <br />
   $ sudo chmod 777 /flashfetch <br />
   
# How to use pfsetmode
1. Creating <app_name>.pf (learning phase) <br />
   $ pfsetmode -r <binary_file_path> <br />
2. Change the mode of an application to prefetch phase
   1) for SSDs <br />
   $ pfsetmode -a <binary_file_path> <br />
   2) for HDDs <br />
   $ pfsetmode -s <binary_file_path> <br />

# Parameter setting (/sys/kernel/debug/flashfetch)
   flashfetch_monitor_timeout    : monitoring timeout (10s for SSDs, 30s for HDDs, by default) <br />
   io_infill_distance_allowed    : I/O distance threshold for the range merge <br />
   io_infill_hole_allowed_blks   : Infill threshold (for reading unneeded blocks) for the infill merge <br />
   metadata_shift_way            : Parallfetch supports two ways of shifting metadata (shifting by I/O counts (0) or size (1)) <br />
   metadata_shift_value          : Shifting size <br />
   rotational                    : 0:SSD, 1:HDD (if you use multiple types of disks, <br />
                                             we recommend you to set this value to the disk type your system partition uses) <br />
   pfault_trace                  : mmap'ed() tracing? (this value should be set to 1 if you want to trace page fault I/Os) <br />
   % Other parameters will be described soon. <br />
   
# Simple example
  < learning phase > <br />
  $ pfsetmode -r /usr/bin/gimp-2.8 <br />
  $ /usr/bin/gimp-2.8 <br />
  < wait 10 seconds > then gimp-2.8.pf is created in /flashfetch directory <br />
  $ pfsetmode -a /usr/bin/gimp-2.8     (if you use HDD as a system disk, use '-s' instead of '-a') <br />
  $ /usr/bin/gimp-2.8 <br />
  <br />
  % if you want to see scheduled prefetch entries <br />
    $ pfviewer /flashfetch/gimp-2.8.pf <br />
  % you need to evaluate performance of Parallfetch, invalidates disk caches to measure the launch time with parallfetch <br />
    (% echo 3 > /proc/sys/vm/drop_caches) as root permission <br />

# We uploaded the source code and document here for paper review.
  We have a plan to upload more source code and detailed documentation here. <br />
  1. Source code for Android, Raspberry Pi3, and Meego
  2. Source code for framework automation (e.g., with nautlius-actions package)
  3. Parallfetch for other kernel versions (we have developed Parallfetch from kernel 2.6.x)
