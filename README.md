# CSaP Project: A AudioMessagge  application

#### Database-Server,Client-Server approch

#### Fully writen in C (Tested on ArchLinux and Pop OS 21.04) .

## Parts:

Client:Interact with the user;

VDR: VoiceDataRepository,is aim is to store and update list of message of various user(his inbox) in an efficent way. 

MDS:Main server, interact with the user for authentication and operate between Client and VDR as intermediary.

## Features

Use PortAudio library (insted of Alsamixer or Pulseaudio ones) for better inter-compatibility.

Auto-crop silence in the message by analyzing peek frequency.

Possibility of search for messages from a specific user, a date range and even the two things tougheter!

Possibility to send long audio without death times .

No Time limit for audio rec has still reconized .

Huge use of semaphore and parent-child processes(use POSIX semaphore insted of system five one)..

The project is highly scalable(you can add how much VDRs you want ), but can still improve(es. Decentralized Mdss that can comunicate each other  ).

Every message is checked for corruption before every tranfer between VDR and MDS or MDS and CLIENT and vice versa.

possibility to remove all the inbox messages with one call

or to delate message one by one. 

change password for logged users implemented.

some security checks in the calls to prevent attacks using a modded version of the source code 

80's style command line interface.



### Release notable problems:

###### ~~lib-portaudio work using alsa on linux  and  sometime i found problems in getting audio by the  microphone off my laptop (Dell G3), anyway i have also  problems  with other app like zoom or google meet~~   (turns out that was *Intel* Sunrise Point-LP HD *Audio*  devicd and  is *Linux* driver snd_hda_intel related problem)

###### maybe recording is a little too verbose and config files too little





### Project structure :

<html>
<head>
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta content="text/html; charset=utf-8" http-equiv="Content-Type">
<meta content="text/css" http-equiv="Content-Style-Type">
<title>CSAP PROJECT</title>
</head>
<body>
<h1 align="center" class="root">
<a name="1r2n2u9bi686kqcgnonb8f4hut">CSAP PROJECT</a>
</h1>
<div align="center" class="globalOverview">
<img src="CSAP PROJECT_files/images/CSAP PROJECT.jpg"></div>
<h2 class="topic">
<a name="0aq7lru5a8j0kgslgqn9bsko2k">CLIENT</a>
</h2>
<h3 class="topic">
<a name="7q7e9gq60cthd56smj5felas1g"> main             </a>
</h3>
<h3 class="topic">
<a name="7p9o5vcjn8frpnnc6n3qjj065q">  main function of the client use do while and loop to switch among options </a>
</h3>
<h3 class="topic">
<a name="2qm37ggmtitmvllulmqnpdnaqn"> addusertoadressbook </a>
</h3>
<h3 class="topic">
<a name="4877putc9jb13bggtp0jt0h9op">  Add a user already register to the local addressbook </a>
</h3>
<h3 class="topic">
<a name="027c8ms8o49u14m00o2lkl8tli"> changepasswd     </a>
</h3>
<h3 class="topic">
<a name="6aubbe6g51k471oervos3a3brr">  change the password of the logged user </a>
</h3>
<h3 class="topic">
<a name="3ducgka2kcerbk5l65cp9km0ln"> checkinbox       </a>
</h3>
<h3 class="topic">
<a name="7oond09755l7o9o002mmpqmttu">  check the number of messagge to read inbox</a>
</h3>
<h3 class="topic">
<a name="3cif8m0ja3vlkfqqalpc8dacq5"> delateMessage    </a>
</h3>
<h3 class="topic">
<a name="0uteniqjieksb5n50phi89av4t">  delate a message from inbox</a>
</h3>
<h3 class="topic">
<a name="5c3qkm69m4g22dmutl4mch2dpu"> disconnect       </a>
</h3>
<h3 class="topic">
<a name="47clpsmt1p8sk21dts5ptu5cal">  exit the app</a>
</h3>
<h3 class="topic">
<a name="053vven63heuekd4fdnrji74m7"> fullwipemessage  </a>
</h3>
<h3 class="topic">
<a name="2fe8ehnhd2sp2g23c73t1apetg">  clear all your inbox</a>
</h3>
<h3 class="topic">
<a name="0kq860ur6nn2c6qu12u6arml3b"> getallmessage    </a>
</h3>
<h3 class="topic">
<a name="1nphg7dkls0et5sdri866efvro">  get all the message in your inbox</a>
</h3>
<h3 class="topic">
<a name="063g7tstk6rn43c9vsob219163"> listallmessage   </a>
</h3>
<h3 class="topic">
<a name="5or0cnr3mqgdj126li4nt9533a">  subroutine to ask and get all the message from mds</a>
</h3>
<h3 class="topic">
<a name="5mad7gi77h16tn6duk3kv94r77"> login            </a>
</h3>
<h3 class="topic">
<a name="2q2bdi441dei7e7fvj0m4383rk">  login with user and passwd</a>
</h3>
<h3 class="topic">
<a name="1qnomd2v5ntcp1k7soe8mnmv53"> selectuserto      </a>
</h3>
<h3 class="topic">
<a name="7c58oiqr03bqhjduq1guq8pgk3">  select a register user from all the register user or from your address book</a>
</h3>
<h3 class="topic">
<a name="217b1i9thj1a7h82b6g98dhh62"> sendNew          </a>
</h3>
<h3 class="topic">
<a name="38ul6n2b60403m1phu1u6qq8he">  send a new message(package data) to another user</a>
</h3>
<h3 class="topic">
<a name="2nfv0kil98tluses3srgh6i02b"> serchMessages    </a>
</h3>
<h3 class="topic">
<a name="173rauv57rc9bu44d4k2ce8cb9">  search a message from your inbox by user ,date or both</a>
</h3>
<h3 class="topic">
<a name="4foa0skj7ral46en1toiecgnim"> showMessagewithOptions </a>
</h3>
<h3 class="topic">
<a name="1tqm7ioa58ra8s0vjep9clf2vu">  display all the info of the message and make it possible to inoltrate it,listen,and delate it </a>
</h3>
<h2 class="topic">
<a name="37t5fa1dquadmp5o5spmeobva2">utils</a>
</h2>
<h3 class="topic">
<a name="079ceu6vamj9olka936s53pre1"> playback   </a>
</h3>
<h3 class="topic">
<a name="3c5toobcdtnq48tfdgpf7vt0f1">  playback audio data structure</a>
</h3>
<h3 class="topic">
<a name="3bcepnvv2esblhnv6k0kl1jopj"> recordP</a>
</h3>
<h3 class="topic">
<a name="260ndn0ifpcokq2oibcrl38p6u">   implementation of the recorder based on the example from portaudio but  can record until a key is pressed and auto remove silence from registration work in parallel using posix thread</a>
</h3>
<h3 class="topic">
<a name="7ti0ba7dn8qjflscn14kn6m5dq">  hashCode </a>
</h3>
<h3 class="topic">
<a name="3a8ogfvin7mi6p0k8tco6gdh5d">  is the hashCode function  for a message</a>
</h3>
<h3 class="topic">
<a name="1lafnfm6hma8p5o3q6dflkgk22">  datecmp</a>
</h3>
<h3 class="topic">
<a name="5gngkfbe343n9bdt0tv5rflk9c">  Compare two dates</a>
</h3>
<h3 class="topic">
<a name="175odmnln5bts4hiebn625s4fk"> send_int</a>
</h3>
<h3 class="topic">
<a name="54vo6b6genedtvpdt52atar640">  send an int trought a socket(usefeull between socket that use different int rappresentation </a>
</h3>
<h3 class="topic">
<a name="2fq23j3logi81hib3q1h7hn95i"> receive_int</a>
</h3>
<h3 class="topic">
<a name="678nhciiub4uhlqa6aq0p845lg">  recive an int trought a socket usefull when socket as two different int rappresentation on the two side </a>
</h3>
<h3 class="topic">
<a name="1tqm195o294nnsjdlupf1jn26q"> send_PackageData</a>
</h3>
<h3 class="topic">
<a name="0tkakfdglbca6h4bjqorlvetuc">  send a Package data(Message) trought a socket</a>
</h3>
<h3 class="topic">
<a name="0nrksvpcoer22gf49defsggda9"> recive_PackageData</a>
</h3>
<h3 class="topic">
<a name="5juat2mmj2m7h1vecdn2fegl8e">  recive a Package data trought a socket</a>
</h3>
<h3 class="topic">
<a name="0o1fb7p754mdbbar924soci63c"> int store_PackageData(PackageData tostore, FILE *fp);</a>
</h3>
<h3 class="topic">
<a name="37a1j8o2ng4vl7pefomjj606k4">  save a package data on a file</a>
</h3>
<h3 class="topic">
<a name="0dqgsldpo4cg16mfdb4t5c1pco"> read_PackageData</a>
</h3>
<h3 class="topic">
<a name="3euivg93chhfd9fac74j1phdlh">  read from a file a package data</a>
</h3>
<h2 class="topic">
<a name="6mgq74hp2mtrnhsi9lbitcrqgr">VDR</a>
</h2>
<h3 class="topic">
<a name="0mado7alu37jag11iit2hb07o3"> main             </a>
</h3>
<h3 class="topic">
<a name="7r18m1ea0cmqbeobvnb8qd45k0">  main function of the vdr is used to connect to the mds send echo and waiting for a call for one of its function</a>
</h3>
<h3 class="topic">
<a name="1fd81hc0mkp68nc38snn9e0isp"> checkuser         </a>
</h3>
<h3 class="topic">
<a name="16ovbdjn3v3lsm57df65bnucle">  check if a user is registered in the vdr</a>
</h3>
<h3 class="topic">
<a name="7imkl7pjsgcqarqiq50g3g3nbm"> delateMessage    </a>
</h3>
<h3 class="topic">
<a name="03hpprm17ffuaumtv14qufho3e">  delate a message from the vdr</a>
</h3>
<h3 class="topic">
<a name="28jmhp0it74knii0ss8celmnrb"> delateallMessages </a>
</h3>
<h3 class="topic">
<a name="44egq5c1s6gtsdng70drbul3ib">  delate all the message of a user from the vdr</a>
</h3>
<h3 class="topic">
<a name="3fboq4ldkqovislf7laqpcjqil"> getinbox         </a>
</h3>
<h3 class="topic">
<a name="7rihm0ml6ngalghi2hsaiu8nme">  get the number of message inbox</a>
</h3>
<h3 class="topic">
<a name="7a64v6su9i58c2t7ntdmmak47n"> readandsendMessages </a>
</h3>
<h3 class="topic">
<a name="3q1kpbmfq924rfi9kpicj2lqa0">  read all the message from a user and send it to the mds</a>
</h3>
<h3 class="topic">
<a name="3gam45bdai18jqpioum9auqpl1"> storeMessage     </a>
</h3>
<h3 class="topic">
<a name="0ngi1dtb1tt3es2g6fsodr918q">  receive a message and save it in the file system </a>
</h3>
<h3 class="topic">
<a name="0l3ilc8tu8g1m0336nos68e3js"> storetofile      </a>
</h3>
<h3 class="topic">
<a name="7mdtblge3sb404g6n3ooonkhqf">  save a message in the file system</a>
</h3>
<h2 class="topic">
<a name="42mn51reimtkol54o6ji7dnpb9">MDS</a>
</h2>
<h3 class="topic">
<a name="5n7ravnj41u8t3h765ttm9c8fl"> main             </a>
</h3>
<h3 class="topic">
<a name="6nvbmafvub0umcp5c9i92jmdne">  main function of the mds open the socket check connections with vdr and waiting for clients when a clients connect create a child process</a>
</h3>
<h3 class="topic">
<a name="1k01nvj1o78nt9k18b5knmrrr9"> dowork           </a>
</h3>
<h3 class="topic">
<a name="5e0gb512gsnj8s8tmiqfa0ndur">  the main activity of a child process that talck with a single user</a>
</h3>
<h3 class="topic">
<a name="2ds987duprre10go1aue4upbtu"> ReciveMessage  </a>
</h3>
<h3 class="topic">
<a name="4dia4nv4rog0bhl8cv76arl07g">  recive a message and send it to the dedicated vdr</a>
</h3>
<h3 class="topic">
<a name="4it6sv45fbd7859h7mt371jrkj"> registeru        </a>
</h3>
<h3 class="topic">
<a name="43oos00bg8h63gvlfio10dl2n1">  register a user </a>
</h3>
<h3 class="topic">
<a name="72ka83v458ph4lk2skgcvvc93r"> autenticate      </a>
</h3>
<h3 class="topic">
<a name="053qiugjgt3946lq8agg030cqd">  authenticate a user </a>
</h3>
<h3 class="topic">
<a name="7p80u98itg1jf9bi6d9uuq8qjh"> changepasswd    </a>
</h3>
<h3 class="topic">
<a name="3lpq0ihlgthpc18umonhm0i9n4">  change the password of a user that request this</a>
</h3>
<h3 class="topic">
<a name="4o83n6qv3vm83dvmed97m53qvp"> delatefromvdr    </a>
</h3>
<h3 class="topic">
<a name="6bs7m0ripsphbga1hngr7chuqg">  delate a message from the delegated vdr</a>
</h3>
<h3 class="topic">
<a name="7f91chg5adu76rp5rct5ived8s"> fullWipeUserFromVdr </a>
</h3>
<h3 class="topic">
<a name="7pcnn2hfv72dd9m9dbm8fl64dm">  full wipe a user data from vdr </a>
</h3>
<h3 class="topic">
<a name="1c8hf645vghj57loq1uu8n9jhp"> getClientMessages </a>
</h3>
<h3 class="topic">
<a name="689csjmn6dcc7b6uuij4578b18">  get all message of a user and send it to the client that was requesting it </a>
</h3>
<h3 class="topic">
<a name="30mpd782dgt8v47b63naut6rdl"> getvdrIndex       </a>
</h3>
<h3 class="topic">
<a name="04b2ocdeos4jcm4ldmb69949o8">  ask to each vdr  if a specific user data is stored inside its memory</a>
</h3>
<h3 class="topic">
<a name="1fic2bkurg7gmevacejcd72jql"> giveInbox         </a>
</h3>
<h3 class="topic">
<a name="2ofaf34qjig1pm677vlf00q7ft">  give the number of inbox message to a user</a>
</h3>
<h3 class="topic">
<a name="1621300e9i81rmh8bsgai1r3pn"> putalluser       </a>
</h3>
<h3 class="topic">
<a name="1dsc9irh4sels3clhs5svo5vng">  send  all the registered user name </a>
</h3>
<h3 class="topic">
<a name="08v73oomaug4mohtapqkmqld8i"> storeMessage    </a>
</h3>
<h3 class="topic">
<a name="04v24fn79bqjdh3158iaevfem7">  store a message from a client inside a vdr</a>
</h3>
</body>
</html>

### Metadata Server (MDS)

#### ● Handles initial connections with clients, maintains a “database” (even implemented as a file) containinginformation regarding users, messages, mailbox status, free space, etc

#### Upon startup, or after a crash, the MDS contacts all known VDR processes, which could be running on thesame or on another server, to get/check information about the status.

#### – Both the MDS and VDR should keep copy of their own metadata.

#### The information wrt IP addresses and port# of the various components is contained in a configuration file,read upon startup by all processes

#### After initialization, the MDS waits for requests from clients which, upon authentication (usingname/password), could:

### 

## ONE or more Voice Data Repositories (VDR)

### ● Store audio files, keeping a copy of metadata (username,timestamp, ..) plus an hash for security.

### ● When starting, they check local files for corruption, rebuild their metadata and “check-in” with MDS

### ● Send/Receive/Delete audio files upon request from MDS (checking hashes before returning them to server)

## 



## Clients

### ● Connect to MDS to authenticate

### ● Check for new messages

### Search for messages:

– From specific users, selecting them from the entire list of from a personal
address book
– Within a specific data range

### ● Retrieve and play messages on the local audio system

### ● Delete messages

### ● Record a voice message and send it to a user (selected as indicated in the above bullet)
