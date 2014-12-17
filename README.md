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
printf 			                                   NULL
printf                     -#^#-                   NULL
printf 			                                   NULL
printf 			                                   NULL
printf (do not use this line yet)	               x


3. Set Object Location
  a. Player
    (1) Make a struct type for Player : have x, y, LiveFlag
    (2) Set init location ( InitialObject() function )
    (3) Put Player image to Screen : Screen[i] = -#^#-
  b. Enemy
    (1) It's simliar with Player but need 30 of Enemy. Enemy[3][10] or Enemy[30]

4. Draw Object
  a. DrawPlayer()
    (1) The warning is that Object Location is little bit different with Draw Location's start position. In other words, the Player's plane is -#^#-. So, it needs 5 position to draw.
    (2) Set Player's Location to Screen[][] array variable to draw player when console draw screen ( while (1){ draw() } part)

--> Current Status is Stop Status

5. Add Action
  a. PlayerAction()
    (1) Use Window APi to check keyborad input
      - GetAsyncKeyState( Parameter )
        VK_UP <- check up keyboard; VK_DOWN; VK_LEFT; VK_RIGHT
    (2) Move (x, y) location by use if statement
      - Becuase GetAsyncKeyState's return value is 16 bit, we should check whether first bit is 1 or not. We gonna deal with this by bit operation &

  b. EnemyAction()
    (1) Enemy's Action is made by Pattern Motion. For example, if we want to move Enemies like fly, we just can set the Pattern in EnemyAction() function.

    (2) All Motion's speed is based on draw() funcion. It means that the while(1) loop in which has sleep() function and draw() works like clock in this game.

6. Shot
  a. CreateShot()
    (1) Shot's Structer
      - Need x, y coordinates, UseFlag, Type
      - x, y coordinates makes computer can compute whether Player or Enemy is shot or not.
      - UseFlag is for calcurate missile count and remove missile.
      - Type tells the missile who shoot.
    (2) CreateShot(Type, x, y)
      - PlayerAction()
        When Player(User) click [Control Key] on the keyboard, Call CreateShot() function
      - EnemyAction()
        I called CreateShot from EnemyAction by using probability.
	if(rand() %100 < 2) CreateShot(E_SHOT, x, y); <== This means that Enemy will shoot 2%.

7. CheckCrush()
  a. Concept
    (1) If PlayerShot and Enemy are in the same x, y coordinates, Enemy will be exploded. In the same way, if EnemyShot and Player are in the same x, y coordinates, Player will be exploded.
  b. CheckList
    (1) Shot[].UseFlag is 1 or 0.
	(2) LiveFlag of Player or Enemy is 1 or 0.
	(3) Enemy should be died by PlayerShot only.
	(4) Check valid range. For instance, Player's y and missile's is easy to check but Player's x and missile's x should check the valid range. In this code, "abs(Shot[].x - Player.x) <= PLAYER_SIZE" is the condition.

8. CheckClear()
  a. Concept
    (1) If Player is died( Player.LiveFlag is 0 ), Print "Game Over"
	(2) If All Enemies is died( Enemy[].LiveFlag is 0 ), Print "Mission Complete!"

한계점
1. 충돌체크
2. Key를 빨리 입력하고 떼었을 때
3. APi사용으로 인해 문자가 남은것 체크 
