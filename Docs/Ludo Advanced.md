Constraints to have all 4 players occupied in the game
Check all metadata of files before uploading
* Board/ structure and type  
* Player/bots  
* Game logic  
* Main


Board

* Array to implement the board  
* Method to return how many players exists   
* Method to return which color exists there  
* Returns where troops are for a specific color  
* Ammount of rounds elapsed (1 round is all players rolls one time)

Player

* Structure to store?  
  * Position (should be accessible by bots and players)  
  * Troops in board 
  * Moves to go for the block before homestrech?
  * Troops at home(end)  
    * We can use the difference to calculate the troops in start cause having a troops at base would make us do a calculation of when a troop comes to the home if all troops are in home now?	

  Current roll number (1,2 or 3rd)

* Troops?  
  	Rotation?  
  	Captured?

		  
		  
	  
Game logic

* Player based logic  
  * Moving a troop move into the board (according to a dice roll of 6\)  
    * Can’t if the roll isnt 6  
      * A Coin flip is executed  
        * Heads- Clockwise  
        * Tails-	Counter-clockwise  
  * Moving a troop (according to a dice roll)  
    * Can’t if no troops exist on the board  
  * If rolls 6 gets to roll again (INCREMENTS ROLL NUMBER)  
    * Current roll number \= 3 then ignore the roll  
* Current roll number is cleared for a player when handing over to the next  
* Can Jump over a player(aka ignored if the landing space doesn’t have a player?)


    
* Eliminating  
  * When eliminated all info of that piece is erased  
  * If landed on a field with one player of the opposing team it is sent to the base  
  * Given an extra roll for the capturer

    

* Landing  
  * If a piece lands on the player itself it creates a block(can be 2 or more)  
    * Opponents can’t jump over   
    * Can land on the adjacent block if the roll is past the block  
    * If there is no other moves to make the turn is skipped		Ex if the roll is 6 and the only player on the board can’t move you have to take out a troop from base  
  * Once in homestretch the troop must roll the exact amount to go to home

* Blocks  
  * Blocks can move together  
  * Can be broken and go back to the normal direction  
  * If from opposite directions all should take the longest route  
    * All are move  (dice roll/number of troops) of blocks  
  * If dice rolls 3 6s then the block should be split in such where all the moves of the troops should sum up to 6

* Block capture  
  * Block of the same size can capture a block(returns to base)  
  * Every troops capturecount increases

			  
		  
		  
		  
		

* Non player based logic  
  * Dice roll  
  * Mystery cell  
    * To appear after two rounds at any cell  
      * Conditions: standard path  
      * Can’t spawn at a place where a troop is situated  
    * Reallocated a mystery cell after 4 rounds  
      * Can’t be in the same location

		

* player/bots  
  * Structure for each player where it has the data stored,  
    * How many in the starting area  
    * Rotation

		  
	NOT WRITTEN YET  
Mystrey cell still has a lot of functions where it isn’t documented please implement after finishing this  
Bots aren’t implemented  


Single pieces can move
  * can move freely if no blocks in path of the roll
  * There can be,
    * Block between the path
      * piece can move to the cell before the block
    * Block at the end or next to it
      * piece can't move

Blocks can move
  * blocks can only split by moving one troop out (ex block of 3 needs 2 turns to get rid of the block)
  * blocks can move but the assigned (roll/number of troops)
  * assume a block can't eliminate a piece or a block of different size
  * a block of same size can be eliminated
    * elimination points will be given to troops equally
  
  * troop should retain,
    * rotation
  * the sum of all moves = rollValue

  (Approach - current) >

  0 allowed
  1 not allowed
  block if equals to myblock
    then eliminate and allowed
    if not not allowed

    check the closest rotation before move
    check the closest rotation after move
      if they change 
          the troop passed the apporoach

  Approach passing function
    a troop can pass the approach
    a block can pass the approach
    