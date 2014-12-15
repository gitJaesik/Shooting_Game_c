############# Make a Shooting Game by C ################

1. Basic Concept Making a Game on Console Window
  a. Get Console's size : Console's size is 80 x 25 without Scroll_bar. So, to prevent Scroll_bar from this console game, we're not going to user 25 Height. (But we gonna use this line for score or something later.) Be sure that Scroll_bar will be made when you use (25, 80) location.
  b. Cursor : To play game clearly, We gonna remove Cursor Point.

2. Drawing : Screen[24][80+1];
  a. I choose using dawing method that use printf
     printf( String ) mean that printf All string before NULL.
     In this case, I can printf all console by using printf 24 times only

     List 1-1 (USE WIDTH 40)

     1234567890123456789012345678901234567890+1
     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
		                             NULL
                     -#^#-                   NULL
		                             NULL
		                             NULL
					    x

 
           1234567890123456789012345678901234567890+1
printf     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
printf     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
printf     @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ @_@ NULL
printf 			                           NULL
printf                     -#^#-                   NULL
printf 			                           NULL
printf 			                           NULL
printf (do not use this line yet)	    x


3. Set Object Location
  a. Player
    (1) Make a struct type for Player : have x, y, LiveFlag
    (2) Set init location
    (3) Put Player image to Screen : Screen[i] = -#^#-
  b. Enemy
    (1) It's simliar with Player but need 30 of Enemy. Enemy[3][10]


