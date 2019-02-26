# Typon

Typon(タイポン|Tai-Pon) is a typing practice tool that runs on a terminal emulator.  
With it, you can 
- [**Test your typing speed**](#regular-game-play)
- [**Keep your records and track your progress**](#track-your-progress)
- [**Practice typing any customized quote**](#customize-your-quotes-directory)
- [**Have some extra fun by contesting your previous records**](#contest-your-previous-best-record)  
  
#### Featured mode: Challenge Mode  
<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/challenge.gif" width="550" height="420">
</p>

You can also
- [**Change The Appearance**](#customize-your-quotes-directory)

---  

## Regular Game Play


<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/intro.gif" width="550" height="420">
</p>


The regular Game Flow of Typon goes like:

> Start Screen -> Typing -> End Screen -> Start Screen -> ...

You can launch **Menu** at almost anytime by pressing the **tab** key.  

The **Shortcut** to every option in **Menu** can be accessed by pressing the **First letter** of each option.  

**a** and **d** are usually synonymous with **Arrow Left/Right**.

**Number keys** can be used to quickly move to the n-th option.

On [**Start Screen**](#start-screen) and [**End Screen**](#end-screen) only, the **hyphen(-)** key will launch **Search mode** and allow your to **jump** to any quote in the current jumble of quotes by inputting its ID.
  

#### Start Screen

On the Start Screen, a quote randomly chosen quote from your **"quotes" directory** and a message asking whether if you would like to begin typing will appear.  

You can press the **first letter** of the quote or **space key** to begin the game. Pressing the **first letter** will save you the reaction time to find and press the first letter on the keyboard which will result in a faster completion.

On the Start Screen only, you can press the **Number key 0** to launch [**Challenge mode**](#contest-your-previous-best-record) or the **delete key** to remove the current quote's **Best record** if it exists. A message asking for confirmation will appear.

#### End Screen

On the **End Screen**, Typon shows all in-game inputs including those incorrect inputs, and also shows the **time**, **number of mistakes**, and **WPM** (word per minute).  
The formula that Typon used to calculate WPM is:

<p align="middle">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/wpm_formula.png">
</p>

You can also press **r key** to **replay** your last Game Play. You can then choose to Skip it by choosing **SkipToEnd** in the **Menu**.

---  

## Track Your progress

On [**Start Screen**](#start-screen) and [**End Screen**](#end-screen), You can access your previous records and the game statistics by selecting the **History** option in **Menu**.

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/history.gif" width="550" height="420">
</p>

In **History**, you can navigate through records by pressing **Arrow Up/Down** or going **Up/Down page by page** by pressing the **w** and **s key**.  

You can **skip** directly to a quote by navigating to its record and press the **Space key**. 

History records can also be **Sorted** by using the **Arrow left** and **right keys** to move across the column, then pressing **Return**. When **Sorting By Different Columns**, if possible, the previous order will be preserved.

**Game Statistics** can be accessed by pressing **0 (zero key)** in **History**.  

- **Typon** currently keeps track of:  
  - All Time Average Gross WPM                     
  - All Time Average Mistakes                      
  - First 20 Games Average Gross WPM             
  - First 20 Games Average Mistakes                 
  - Recent 20 Games Average Gross WPM              
  - Recent 20 Games Average Mistakes               
  - Number of Games Played                    
  - All Time In-Game Key Pressed 

---  

## Customize Your Quotes Directory

Typon can read in any **text file(.txt)** and let you practice typing on it, but the max length of your text file may vary depending on the terminal window's size. 
In order to make your text files accessible by Typon, you need to store them in a directory called **quotes** under **Typon**'s root directory and name them following a certain naming rule.


<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/custom%20quote.gif" width="550" height="420">
</p>  


#### Quote Naming Rules:

- Name of the text file must be of the form: **quote[ID].txt**
- **ID** must be a non-negative integer ranging: **\[0, MAX_ID)**
- **MAX_ID**, by default, is set to be: **3001**

**Note**: I didn't feel the necessity for making MAX_ID an optional value, however, it can be easily modified in the source code. 

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/file_struct.png" width="550" height="320">
</p>  

#### Be Careful:  

Any file that is not in "**.txt**" format, or does not follow the aforementioned naming rules will be ignored by **Typon**.

Since **Typon** doesn't keep a copy of the content of all the text files, it usually only recognizes the **quote ID**. If the ID isn't changed, it assumes that the content is the same.

**Typon** checks the text file's content, only when trying to start a game under [**challenge mode**](#contest-your-previous-best-record), since incorrect record may cause undefined behaviour in this mode.


## Contest Your Previous Best Record

If you start a Game under **Challenge Mode**, the opponent's text cursor will appear and start a **Race** with you.  
If you manage to finish before the Opponent does, **Congratulations**, you've beaten your previous **Best record**.


## Change The Appearance

#### Resize Typon

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/resize.gif" width="550" height="420">
</p>

When the quote that you're trying to play is too long to display, you may have to resize **Typon**.

There're **two** different ways to resize **Typon**:
1. Resize terminal window before launching **Typon**  
2. Use the `-resize[width][height]` flag

**Option 1**: Let **Typon** automatically fit to the Terminal window's size, if Terminal is smaller than `width: 110` x `height:110`.  
**Option 2**: Force **Typon** to be exactly the size specified by input arguments, and it can resize **Typon** to larger than `width: 110` x `height:110`.  
  
**Note**: Either way, you can't resize Typon to larger than current Terminal window's size, or smaller than  
`Absolute minimum window width: 55` x `Absolute minimum windoe height: 20`. Forcely resizing **Typon** to be smaller than it's current size will cause **Typon** to quit.  

#### Change Typon's color setting

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/changecolor.gif" width="550" height="420">
</p>

**Typon** only uses the **8 basic colors** on Terminal, namely:
- Black 
- Red
- Green
- Yellow 
- Blue
- Magenta
- Cyan
- White  

Current version can only change **Typon**'s color setting by modifying Terminal color definition.



