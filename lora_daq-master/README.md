# LORA_DAQ

Notes:


DIAGNOSTICS LOG FILE:
One Entry every 120s
For Each Station: N Triggers since last log
LAST LOFAR TRIGGER TIME STAMP


LOFAR TRIGGER LOG FILE:
AS IT IS.


Main socket:

1. Calib: Send 99 30 80 66 via main socket.
2. Normal: Send Control_Messages via main socket
3. Calib: Receive calib data (Get_Data.h)
4. Normal: Receive data (Get_Data.h)

Spare socket used for:

1. Setting Threshold within While(1) loop.Based on running avg of noise.
2. Reset_Electronics()
3. Stop_LORA()

Sequence of actions after Connected to stations:



LORA Data Acquisition Software For Server and Clients.

Basic Idea:

Each socket loads information into a circular buffer.
As soon as a packet with Header + Tail char bits is found,
that packet is passed on to a buffer for Event/Log/Noise.

For Event, buffer should be large enough to hold 200 events.
Log , Noise buffers only need to hold one msg each. the buffer gets overwritten with new incoming msgs.

These Event/Log/Noise buffers pass on data to a more organized array-wide organized containers, which then get written into root files.



CLASS Descriptions:

LORA:  Base Class
*LORA_V2 will inherit LORA. (new stations)
*LORA_V1 will inherit LORA. (old stations)

*new and old form the interface between funkyness of old, new stations and more uniform Event/ Log/ Noise containers maintained by OPERATIONS class.

*methods: Init(), Interpret() checks circular buffer and sends information to appropriate containers i.e. interprets the packet received via socket,...  Open, Close, Accept, Listen, Bind,....

Sequence of class calls:

1. Ops.Init(network file, config file)
* loads network, config information into appropriate class containers.
* adds elements to lora_array vector of pointers to LORA base class.
* calls LORA.Init() for each station.

2. Ops.Open_Connections()


# recommended initial thresh low values should be slightly on the higher side.
# if within first few mins station fails to send triggers, it will be lowered.
# if you set it too low, i.e. below baseline, the stations dont trigger at all!
# against setting it a bit higher where rate of high E showers might be low.

Reference URLS:
* https://stackoverflow.com/questions/40978921/how-to-add-chmod-permissions-to-file-in-git
* https://eklitzke.org/blocking-io-nonblocking-io-and-epoll
* https://stackoverflow.com/questions/589783/not-able-to-catch-sigint-signal-while-using-select


Hershal's log:

note to self:
compare from this file: Run Start Time: 2019-10-22 7:40:52
and from this file: Run Start Time: 2019-10-22 12:39:29
stop old daq and re-start - it will save the root file. just change the init threh values so its quick to restart.

Run With Directly using New DAQ: Run Start Time: 2019-10-22 14:32:55

Run With Directly using New DAQ, removed 10ms wait time in Listen() in OPs class.: Run Start Time: 2019-10-22 15:2:14


Removed listen until EWOULDBLOCK for individual sockets. They will only be listened once in one iteration. So next socket is heard with information of the same event. Otherwise a single socket supplies multiple event informations before next socket is read out. Creating split events. (Ansatz).
Also introduced a check that last hit in all_hits is not within timewindow. that means it should collect more hits before forming event.

Finally, use this to compare with 2/4 file: Run Start Time: 2019-10-22 17:25:10

Need to run again in relay mode. Earlier files are useless due to event splitting.

Comparison for Relay mode: Run Start Time: 2019-10-22 18:43:3 . Old DAQ started at same time. After the improvements listed above to avoid event splits.


—missed trigger due to splitting—
: 6:6:47, 459396805(look below) current event (v): 6:6:47, 459396676
 6:7:9, 895156176(look below) current event (v): 6:6:47, 459396861
 6:7:11, 995443163(look below) current event (v): 6:6:47, 802943789


459396676
459396805
459396861
802943789


— FINALLY!!!! Run Start Time: 2019-10-23 12:24:48 file can be matched with old daq. And the file right before this one.

— Normal run using new daq: Run Start Time: 2019-10-23 14:2:59

— RUN WITH SIGMA OVR THRESH 4.5 and lofar trig cond 6 so it never sends: Run Start Time: 2019-10-23 14:53:38

—RUN WITH TRIG COND 2/4: Run Start Time: 2019-10-23 16:10:15 ( station 4 stopped !)

—RUN WITH TRIG COND 2/4 again: Station 4 keeps stopping. There should be several runs for this case.

—NORMAL RUN to match with old daq (relay mode):  Run Start Time: 2019-10-23 17:49:49 ( had radio triggers… better for comparison than 12.24.48 one above) .


Edits:
Relay server EWOULDBLOCK removed
increased coinc check interval to 600ms
—NORMAL run to match with old daq (relay mode) 20191030_112815 .

Restructured root file
increased check coinc interval to 1000 ms
increase wait_another_iteration to 250 ms
avoid comparing initial 30 minutes while thresholds settle down.

changed in old daq relay version:
NO Entries changed to 10 from 2. Because it only uses last 2 (was 100 intially)… but also wait to collect 2 (was 100 initially..) . really bad logic.!
reset interval 0.083 instead of 0.05
changed Lasa Settings init thresholds to 25. (down by 5 from 30)

—NORMAL run to match:Run Start Time: 2019-11-1 1:15:57 - if it works. somehow relay is failing by new daq losing connection to relayed sockets.


sat 2 nov 2019 01:37 am.

rebooted lora main pc, rebooted lora4, lora5 (and yday i had rebooted lora1).
i changed the Lasa_Settings file to bring initial threshold values down by a bit more.

—NORMAL run to match: a bit before… Last Refresh UTC Time: 2019-11-02 00:38:27 with 20191102_02406.root in old daq.


3.30 pm sat 2 nov 2019:
Ended the above run at around 3.30 pm CET nov2nd.
new DAQ changed lofar trig cond to 16 detectors .
changed reset thresh interval to 13 minutes…approx.
Started new DAQ directly without relay… now - at about 3:30 pm aka. UTC time… 2019-11-2 14:32:58 …

20.29 sat 2 nov 2019:
no triggers were being received by astron
so changed trig condition to 14 panels
and started again.
new DAQ without relay: Run Start Time: 2019-11-2 19:29:26



sun 3 nov 2019 11.30 am

issue with new DAQ: events were being splitted.
changed Interpret_and_Store_msgs for LORA_STATION_V1
interpreting msgs in the buffer until its empty

sun 3 nov 2019 12.51 pm
changed Get_Buffer_Size<=min_msg_size instead of just "<"
moved intepret of LORA_STATION_V1 inside the listen()  of ops class.
Run Start Time: 2019-11-3 11:51:6

didn't solve the problem of event splitting.
changed select() timeout to 10 microseconds.
and removed extra wait.

19.34 pm
put Listen_To_Stations() in loop to repeat 5 times.
In each main() loop.
put std cout for check coinc interval. actually happening.
after all the returns + wait_another_iteration etc.

19.47 pm
doing Listen_To_Stations() for 1 second. (instead of 5 times)
logic failed.

20.03 pm
doing  Listen_To_Stations() for 100 times with 10ms wait time in select()
so effectively doing Listen_To_Stations for  1000 ms
Run Start Time: 2019-11-3 19:0:55

21.43
issue not resolved
made 500 times. for Listen_To_Stations
 Run Start Time:  2019-11-3 20:44:25
 no need to see root file. deosnt seem to work. grrr.

22.37
issue not resolved... but after this iteration. check root files.
moved Interpret out of Listen_To_Stations()
made the counts for listen 100. added 1 ms wait time .
printing out the buffer size of each socket.

04 nov 2019 2.00 am

issue resolved. the damned event_msg_size was wrong in my code.
god knows how i cooked it up.
its 12023 not 12284.

04 nov 2019 11.53 am

file with prefix:  20191104_1051
normal run in relay mode.

04 nov 18.50 pm
new daq direct run: Run Start Time: 2019-11-4 18:59:40
trigger is received by LOFAR!! :D
ran for 1 hour approx.

05 nov 03.44 am
included config trees in the root file
change trigger condition to 15 panels
testing new daq direct run for few hours.
(so that later on we can compare with 2/4 or lower threshold runs)
Run Start Time: 2019-11-5 3:7:47 (good run until it goes crazy after 3 or 4 hrs. see det_log file)

09,36 am
something went terribly wrong. or a simple thing like detectors shutting off
most likely the buffer got too big. or something like that.
so the code starts to behave as if there is no msgs.
rather than stations actually shuttig off.
check screen shots in desktop folder.
load this log file in dashboard and you will see the baseline
and n trigs jumpying ooff the roof for det 16 .
god knows what happened... have to run the code again.
will add sanity function. to check time_since_last_event, time_since_last_osm.
dont know if this should kill the code or what. ?
Run Start Time: 2019-11-5 8:36:24 (good run for about 2 hrs)


10.35 am
the earlier run started at 3.7 , i think ran okay until 7 am UTC or 6 am UTC.
see when last radio trigger was. look at detectors_log file.
changing trig condition to 2/4  
reduced reset thresh interval to 3 mins.
Run Start Time: 2019-11-5 10:21:16
effective check_coinc_interval too high. not able to keep up
with event rate.

11.30
reduce for loop iterations from 100 to 5 for listen method in main.
i.e. isntead of 1 second its going to take 50 ms to do this step in main loop.
reduced check coinc itnerval to 500 ms
so effectively it brought down the check_coinc_interval from 1 sec to 0.5
Run Start Time: 2019-11-5 10:37:12

occassionally when spools are not equal, i can see spool size getting larger and larger. need to reduce the check coinc interval further.
made it 150 ms. and removed for loop for listen.
changed init reset thresh interval to 30 seconds.
and final reset thresh interval to 60 seconds.

12.28 pm
make it only lasa 1.
increase hundred_means / sigmas ... size to 1000 .
reduce threshold to 3.0 sigma.

12.41 pm
added a condition that only wait_another_iteration if event_spool[0].size()<4
otherwise go on and construct events.
WARNING: bad edit... wait_another_iteration removed.
sigma_ovr_thresh 4.0

15.59 pm
ran in det calib mode with calib_listdets: 2,3,4 and only lasa1 in active stations
6.0 sigma works. about 6 hz of event rate. one hour data is 1.3 mb for 1 station.
5.0 sigma too high rate, check_coinc_interval of 50 ms not able to keep up.
5.5 sigma.-- nope! too high rate.
5.8 sigma... nope. dont know why.
4.5 sigma and 2/4 with det 1 and 3 triggering. 2 and 4 with 1000 thresh -- works but rate too low.
making it 3.5 sigma. 2/4 . and det 1 and 3 triggering.
back to 1/4. det 1 trig. 2/3/4 with 1000 adc. 6.0 sigma. works.
Run Start Time: 2019-11-5 15:23:51 run ongoing.... to collect accidental muons in 2/3/4
stopped this run .

removed forced wait time in listen in ops class. instead made select timeout to 10ms.

6.5 sigma also runs out of buffer. running with 7.0 sigma. that should be low rate. lets see.
too high rate. 1/4 trig won't work out.

18.28 pm
changed to 1 and 2 triggering. with 2/4 condition.
They are close to each other so probably will trigger more.
4.5 sigma.
added peaks to the array log file... getting too big. we'll see.
increased check_coinc_interval to 100ms


19.01
trig on 2
1/3/4 readout only.
1/4 trig.
5.0 sigma
not saving events in root file.
worked wonders....
USE these two log files for charge histogram:
 20191105_1919
 and  20191105_1814 .

 21.35
 trig on 2
 1/3/4 readout only.
 1/4 trig
 5.0 sigma
 added status monitoring/ removed as well...
and ... saving root file.
worked well.

06 nov 2019 08.09 am
Run Start Time: 2019-11-6 7:3:29
5 triggering
6/7/8 readout.
5.0 sigma
1/4 trig
the panel 5 miraculously gives muon spectrum in 5 minutes!!

06 nov 2019 08.09 am
5/6/8 readout
7 triggering
5.0 sigma
1/4 trig.
not happening same with det  7
but letting this run for couple hours to collect enough for det 5
to show that its spectrum obtained earlier was correct.
no 7 has a nice muon spectrum by 7.40 UTC.
code went mad... only 2 ch's were being reported into the event.
also, noticed that even though threh for det 7 was 38ish... the peak histogram went all the way down to 6-10 ADC.

10.26 am
5/6/8 readout
7 triggering
5.0 sigma
1/4 trig.
increased super high threshold values to 4000 mv aka 8000 ADC.
(katie says 1000mv / 2000ADC is only quarter or half way to saturation.)
nope, the complete charge histogram also happens now. peak histogram min is 6-10 like before.
Run Start Time: 2019-11-6 9:26:44


10.26 am
5/6/8 readout
7 triggering
9.0 sigma
1/4 trig.
increased super high threshold values to 4000 mv aka 8000 ADC.
(katie says 1000mv / 2000ADC is only quarter or half way to saturation.)
nope, the complete charge histogram also happens now. peak histogram min is 6-10 like before.
Run Start Time: 2019-11-6 9:26:44


22.13 pm
lasa 1
2/3/4 readout only
1 triggering
2.0 sigma
to see if PE peaks are visible...


DOCUMENT THIS:

// << " max event rate limited by check coinc interval. introduce check. " << std::endl
// << " if events are 60 s apart. its 60s before check coinc happens. delaying LOFAR trig decision. introduce a check. " << std::endl
// << " every time a log msg comes in , add wait time to check coinc. because it messes up sequence of incoming hits." << std::endl
// << " But make it move on if next hit doesn't show up in 45s.  " << std::endlDocument it.no need to implement.
// << " reexamin : request_OPS_to_wait_another_iteration<=3)" << std::endl

// Even though threshold is reset every 30 mins. Its avged over 60 mins.Document it.

if reset_thresh_interval= 30 mins, and init_reset_thresh_interval=3 mins,

for first 30 mins, the reset interval will stay at 3 mins.

// move to screen from vnc. have 5 screen sessions for 5 stations permanently open.
// just send sh start lasa client to corresponding screen session.
// set up a script that opens 6 lora pc windows with 6 screeen sessions on.

reset threshold:
for the first half hour, it resets every 3 mins.
then after that every half hour.
half hour = std reset log_interval
3 mins = init reset interval.
6 mins delay between two triggers.


you can run the main.cxx for elec calib as well as muon calib.
by providing appropriate det config files.
but... the other two scripts are provided
for a more clean implementation for elec calib. since it doesn't need most modules.
and muon calib for automated calibration of all the panels in a loop.


installation:
go to build/
run cmake ../
run make.
thats it.

# usage of code:

## in auto elec calib mode aka THE PUPPETEER:
0. attach to screen session named THEPUPPETEER
1. kill bash script thats running.
1. run 'sh stop_lora.sh'
2. do screen -ls , check only one session named LORAMAIN is active.
3. do screen -r LORAMAIN and check its been cd'ed inside build directory. and that daq is not still active.
4. detach from LORA main ctrl+a d
5. edit the input/detector_config.txt as you like.
6. sh the_puppeteer.sh

## normal mode without auto elec calib:
0. check available screen sessions. THEPUPPETEER and LORAMAIN should not have active processes in them.
1. edit the input/detector_config.txt as you like.
2. cd to build/ and start ./main
3. cd to daq_managers/ and sh input/stop_lora.sh to stop the code.

## todo:
1. lasa1_is_active ... etc are incorrect. Check root file to see if its a pyroot problem.  then fix it in cpp code if needed.
2. speed up dash by show/hide for extra plots: https://community.plot.ly/t/dash-slow-to-reload/4645/4

# MUON CALIB NOTES:
27 Nov 2019
## List of sigma's to run:

### ENDED UP RUNNING ALL 20 PANELS again
### 2.4 to 4.8 in steps of 0.12 for 7 minutes each. (instead of previous 6 minutes)

1. Det 0+1: 2.6, 2.7, 2.8
2. Det 1+1: 2.45, 2.6, 2.7, 2.8
3. Det 2+1: 3.45, 3.5, 3.55, 3.6, 3.65
4. Det 3+1: 3.3,3.4,3.5,3.6

5. Det 4+1: 3.9, 4.1, 4.3
6. Det 5+1: 3.8, 3.9, 4.1
7. Det 6+1: 3.9, 4.1
8. Det 7+1 : 3.9, 4.1, 4.3

9. Det 8+1: 3.2, 3.3, 3.4, 3.6
10. Det 9+1: 3.4, 3.6
11. Det 10+1: 2.9, 3.1, 3.3
12. Det 11+1: 2.9, 3.1, 3.35,

13. Det 12+1: 2.4, 2.55, 2.65
14. Det 13+1: 2.65, 2.7,
15. Det 14+1 : 4.3 ,4.4, 4.5, 4.6, 4.7, 4.8
16. Det 15+1 : 4.4, 4.6, 4.8, ,5.0

17. Det 16+1 : 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.2
18. Det 17+1 : 3.6, 3.8, 4.0, 4.2, 4.4,
19. Det 18+1 : 2.4 to 4.6 in steps of .2
20. Det 19+1 : 2.4 to 4.6 in steps of .2

## List of final runs selected:
