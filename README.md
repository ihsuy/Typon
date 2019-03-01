<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/logo2.png" width="350">
</p>

# Typon

Typon(タイポン|Tai-Pon) is a typing practice tool that runs on a terminal emulator.  


<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/intro.gif" height="370">
</p>

#

**With it, you can**  
- [**Test your typing speed**](#regular-game-play)
- [**Keep your records and track your progress**](#track-your-progress)
- [**Practice typing ANY customized quote**](#customize-your-quotes-directory) 
- [**Have some extra fun by contesting your previous records**](#contest-your-previous-best-record)  

## Featured Mode:  Challenge Mode 

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/challenge%20showcase.gif" height="26">
</p>

If you start a Game under **Challenge Mode**, the opponent's text cursor will appear and start a **Race** with you.  
If you manage to finish before the opponent does, **Congratulations!!!**, you've beaten your previous **Best record**. 

For more **details** about the **Challenge Mode** [**click here**](#contest-your-previous-best-record) .

#

### Features  

- **Completely offline**

- **Accepts literally [any quote](#more-about-text-file-contents) and always preserves the format.**

- **Works purely on terminal**

- **No complicated [installation](#installation) necessary**

- **Compiles and runs without changes on most common flavors of Unix (MacOS included)**


#

## Installation

### Dependencies

[NCurses](https://www.gnu.org/software/ncurses) 

### Install From Source

Open your terminal in your preferred directory and clone this repository:
```sh
git clone https://github.com/ihsuy/Typon
```
Run the makefile
```sh
cd Typon

make
```

**Optional:**  

After make, you can choose to install Typon to your `usr/local/bin` directory and run **Typon** from anywhere:
```sh
make install
```
> **Note**: a folder containing 100 random quotes is included, feel free to [add new ones or modify the old ones](#customize-your-quotes-directory).  

**You're all set, enjoy typing!**

---  

## Regular Game Play

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/intro.gif" height="370">
</p>

The Typon's regular game flow goes like:

> Start Screen -> Typing -> End Screen -> Start Screen -> ...

### About Menu

You can launch **Menu** at almost anytime by pressing 
> **`tab`**  

The **Shortcut** to every option in **Menu** can be accessed by pressing the **First letter** of each option.  

**`a`** and **`d`** are usually synonymous with **`Arrow Left/Right`**.

**`Number keys`** can be used to quickly move to the n-th option.  

#

### Start Screen

The Start Screen will be shown before each game begins.

On the Start Screen, a quote chosen from your [**"quotes" directory**](#typon-file-structure) and a message asking whether if you would like to begin typing will appear.  

You can press the **first letter** of the quote or **space key** to begin the game. Pressing the **first letter** will save you the reaction time to find and press the first letter on the keyboard which will result in faster completion.

### End Screen

The End Screen will be shown before each game completes.

On the **End Screen**, Typon shows all in-game inputs including those incorrect inputs, and also shows the **time**, **number of mistakes**, and **WPM** (word per minute).  
The formula that Typon used to calculate WPM is:

<p align="middle">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/wpm_formula.png">
</p>

#
#### Watch How You Did: Replay

On [**End Screen**](#end-screen), to view your last game play, press
> **`r`**  
>
> **Note**: You can then choose to skip the replay by choosing **SkipToEnd** in the **Menu**.

#### Manually Select Quote: Goto

On [**Start Screen**](#start-screen) and [**End Screen**](#end-screen) only,   
to **jump** to any quote in the current jumble of quotes by inputting its ID
> **`hyphen (-)`** 
>
> **Note**: A prompt will appear.

---  

## Track Your progress

After each game is completed, **Typon** saves records about **Quote ID**, **WPM**, **Number of Mistakes** and **Date**.  
On [**Start Screen**](#start-screen) and [**End Screen**](#end-screen), You can access them and the game statistics by selecting the **History** option in **Menu**.


<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/history.gif" width="550" height="420">
</p>


### In history, you can

**Navigate through records**
> **`Arrow Up/Down`**  

**Going Up/Down page by page**
> **`w`** or **`s`**  

**Jump directly to a quote by navigating to its record and press**
> **`Space`**  

**Move across the column**
> **`Arrow Left/Right`**  

**Sort by column**
> **`Return`**   
>
> **Note**: When **Sorting By Different Columns**, if possible, the previous order will be **preserved**.

**Check game statistics** 
> **`0`**
>
>
>  **Note**: **Typon** collects statistics on:  
>
>  - All Time Average WPM                     
>  - All Time Average Mistakes                      
>  - First 20 Games Average WPM             
>  - First 20 Games Average Mistakes                 
>  - Recent 20 Games Average WPM              
>  - Recent 20 Games Average Mistakes               
>  - Number of Games Played                    
>  - All Time In-Game Key Pressed 

---  

## Contest Your Previous Best Record

**Typon** keep track of every detail of every valid input in each game, but only saves the details of your **Fastest Game Play** which will be used in the **Challenge mode**.  

Under this mode, the opponent's text cursor (Your previous best record) will appear and start a race with you. 

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/challenge.gif" width="550" height="420">
</p>

If the **Best Record** exists for the current quote ID,
**You can launch Challenge mode on the [Start Screen](#start-screen) by pressing**
> **`0`**  

**Or remove the current quote's Best Record by pressing** 
> **`delete`**  
>
> **Note**: Removal of the best record will temporarily **disable** Challenge Mode for this quote.  
>           A message asking for confirmation will appear.

---  
## Customize Your Quotes Directory

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/custom%20quote.gif" width="550" height="420">
</p>  


Typon can read in any text file(.txt) and lets you practice typing with it, but the max length of your text file may vary depending on the terminal window's size. ([resize Typon window](#changing-appearance))

#### Typon File Structure

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/file%20struct.png" width="550" height="320">
</p>  

As demonstrated above, in order to make your text files accessible by Typon, you need to store them in the **quotes directory** under **Typon**'s root directory and title them according to a certain [naming rules](#quote-naming-rules).

If you forgot your default Typon root directory , use this flag to locate it:  
```
typon -path
```  

The `-path [user defined path]` can also be used to manually set where typon search its quotes directory and save files:  
```
typon -path /Users/me/new_location/
```  
> **Note**: It is not recommended to change the path to Typon's root directory.


#### Quote Naming Rules

- Name of the text file must be of the form: **quote[ID].txt**
- **ID** must be a non-negative integer ranging: **\[0, MAX_ID)**
- **MAX_ID**, by default, is set to be: **3001**

> **Note**: I didn't feel the necessity for making MAX_ID an optional value, however, it can be easily modified in the 
>           source code. 

Any text files that are not in [**Typon's quotes directory**](#typon-file-structure) or does not follow the aforementioned [**naming rules**](#quote-naming-rules) will be ignored by Typon.

Since Typon doesn't keep a copy of the content of all the text files, it only recognizes the **quote ID**. If the ID isn't changed, Typon assumes that its contents remain the same.

> **Note**: Typon checks the text file's content, only when the user tries to start a game under [**challenge mode**](#contest-your-previous-best-record), 
>           since the incorrect record may result in undefined behaviour.

#

#### More About Text File Contents
If the window's size allows, Typon accepts **ANY** content as long as its in English Language.
If possible Typon also preserves the original format of your text file.

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/monalisa.png" width="500" height="520">
</p>  

**Content Prettify**: If a line is too long, Typon will start a new line at an appropriate postion to avoid disjointed words unless the word is wider than the window which is very unlikely.

## Changing Appearance

#### Resize Typon

<p align="left">
  <img src="https://github.com/ihsuy/Typon/blob/master/demo_gif/resize.gif" width="550" height="420">
</p>

When the quote that you're trying to use is too long to display, you may have to resize **Typon**.

There are **two** different ways to resize **Typon**:
1. Resizing the terminal window before launching **Typon** 
> **Note**: This lets Typon automatically fit into the Terminal window. (won't be larger than `width: 110` x `height:110`)  
2. Use the `-resize[width][height]` flag, for example:
```
typon -resize 90 30
```
> **Note**: This forces **Typon** to be exactly the size specified by the input, and it can resize **Typon** to any size as long             as you have a large enough Terminal window.  
  
**Note**: Either way, you can't resize Typon to larger than the current Terminal window's size, or smaller than  
`Absolute minimum window width: 55` x `Absolute minimum window height: 20`.  
Forceably resizing **Typon** to an invalid size will cause **Typon** to quit.  

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

Current versions only support changing **Typon**'s color setting by modifying Terminal's color definition.

## Credits

Heartful thank you to: 
- Anna for great suggestions and tips. :heart: 
- Kyu for correcting my broken English. :muscle:  
- [shadowmourne](https://github.com/shadowmourne) for very helpful coding advices. :eyeglasses:

## License
Copyright (c) Yushi Qiu. All rights reserved.  
Licensed under the GNU General Public License v3.0.
