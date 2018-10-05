<html>
<body>
<h2>sipcmd2 - simple SIP command line softphone</h2>
<h3>Introduction</h3>
<p>
Updated sipcmd command line soft phone that makes phone calls, accepts calls, enters DTMF digits, plays back WAV files and records them. A useful testing tool for VoIP systems. Runs on Linux.
</p>
<h3>Install</h3>
<h4>Dependencies</h4>
<p>
<h5>Amazon Linux 2 / RHEL 7 / CentOS 7</h5>
<code> 
 yum install opal-devel ptlib-devel && yum groupinstall 'Development Tools'
</code>
<h5>Ubuntu</h5>
<code> 
 apt-get install opal-dev ptlib-dev 
</code>
<h5>Ubuntu 14.10</h5>
<code> 
 apt-get install libopal-dev
</code>
<h5>Ubuntu 12.10</h5>
<code>apt-get install libpt-dev libopal-dev</code>
</p>
<h5>Debian</h5>
<code> 
 apt-get install libopal-dev libpt-dev 
</code>
<br>
<br>
<h4>Download</h4>
<code>
git clone https://github.com/guisousanunes/sipcmd2.git
</code>
<br>
<br>
<h3>Compile</h3>
<p>
<code> make </code>
<br>
<br>
(To disable debug messages, comment out DEBUG flag from Makefile)
</p>

<h3>Environment</h3>
<p>
If you compile the dependencies from source, make sure that libpt and libopal are in your <code>LD_LIBRARY_PATH</code>. The default installation location is <code>/usr/local/lib</code>.
</p>

<h3>Run</h3>
<b>Run options:</b>
<pre>
-u <name> --user <name>         username (required)
-c <passw> --password <passw>   password for registration
-a <name> --alias <name>        username alias
-l <addr> --localaddress <addr> local address to listen on
-o <file> --opallog <file>      enable extra opal library logging to file
-p <port> --listenport <port>   the port to listen on
-P <proto> --protocol <proto>   sip/h323/rtp (required)
-r <nmbr> --remoteparty <nmbr>  the party to call to
-x <prog> --execute <prog>      program to follow
-d <prfx> --audio-prefix <prfx> recorded audio filename prefix
-f <file> --file <file>         the name of played sound file
-g <addr> --gatekeeper <addr>   gatekeeper to use
-w <addr> --gateway <addr>      gateway to use
</pre>
<p>
<code>-l</code> or <code>-p</code> without <code>-x</code> assumes answer mode. Additional <code>-r</code> forces caller id checking. <code>-r</code> without <code>-l</code>, <code>-p</code> or <code>-x</code> assumes call mode.
<br>
To register to a gateaway, specify <code>-c</code>, <code>-g</code> and <code>-w</code>
<br><b>Example:</b><br><br>
<code>
./sipcmd -P sip -u [username] -c [password] -w [server] -x "c<number>;w200;d12345"
</code>
</p>

<br>
<b>WAV file requirements:</b>
<ul>
<li>mono
<li> 8 kHz sampling rate
<li> 16 bits sample size
</ul>

<b>The EBNF definition of the program syntax:</b>
<pre>
prog	:=  cmd ';' <prog> |
cmd	:=  call | answer | hangup
	  | dtmf | voice | record | wait
	  | setlabel | loop
call	:=  'c' remoteparty
answer	:=  'a' [ expectedremoteparty ]
hangup	:=  'h'
dtmf	:=  'd' digits
voice	:=  'v' audiofile
record	:=  'r' [ append ] [ silence ] [ iter ] millis audiofile
append	:=  'a'
silence	:=  's'
closed	:=  'c'
iter	:=  'i'
activity:=  'a'
wait	:=  'w' [ activity | silence ] [ closed ] millis
setlabel:=  'l' label
loop	:=  'j' [ how-many-times ] [ 'l' label ]
</pre>
<b>Examples:</b><br><br>
<code>
"l4;c333;ws3000;d123;w200;lthrice;ws1000;vaudio;rsi4000f.out;j3lthrice;h;w2000;j4"
</code>
<br><br>
Parses to the following:
<ol>
  <li> do this four times:
    <ol> 
      <li> call to 333
      <li> wait until silent (max 3000 ms)
      <li> send dtmf digits 123
      <li> wait 200 ms 
      <li> do this three times:
      <ol>
        <li> wait until silent (max 1000 ms)
        <li> send sound file 'audio'
        <li> record until silent (max 4000 ms) to files 'f-[0-3]-[0-2].out'
      </ol>
      <li> hangup
      <li> wait 2000 ms
  </ol>
</ol>
<br><br>
<code>
"lstart;a;lwai;w1000;j10lwai;d5;lafterwai;w1000;j10lafterwai;h;j0lstart"
</code>
<br><br>
Parses to the following:
<ol>
  <li> do this infinity times:
    <ol> 
      <li> wait for a call and answer
      <li> wait 10 s (using label and jump)
      <li> send dtmf 5
      <li> wait 10 s (using label and jump)
      <li> do this three times:
      <li> hangup
  </ol>
</ol>
<hr>
<br>
Credits for sipcmd creator tmakkonen in project <a href="https://github.com/tmakkonen/sipcmd">sipcmd</a>.
</body>
</html>
