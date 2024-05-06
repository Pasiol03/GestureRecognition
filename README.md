
# Instructions - Very Important

## Unreal Engine

Install Unreal Engine Version 5.3.2

Download MetaXR Plugin for the Meta Quest from the Unreal Marketplace or use this link:
https://www.unrealengine.com/marketplace/en-US/product/8313d8d7e7cf4e03a33e79eb757bccba

Install Plugin to Engine


## Quest

Download and install Meta Quest Link App:
https://www.meta.com/de-de/help/quest/articles/headsets-and-accessories/oculus-rift-s/install-app-for-link/

Connect the Quest with the computer

Use the Meta Quest 2, 3 or Pro


## Code

To properly run the project follow the steps

### 1. Generate Solution
 Generate Visual Studio Project Files to get access to the code

### Adjust absolute Paths

Handtracker.cpp: 

    1. adjust from line 107 to 137 the paths for the ***_Fig*.txt and *model_**.txt files
    2. adjust the testing_data and testing_result.txt absolute path in line 218, 224

hmm.cpp:
    1. adjust testing_answer path in line 216


### Adjust number of (existing) models 

In Handtracker.cpp remove models from the HMM_Models list at line 202

and adjust eventually the naming of the models in the Handtracker.h file at line 45

### Train a new Gesture

Copy one of the model_xx.txt files and rename it to model_xx+1.txt

Create new Sequenze_Figx.txt file to store the data.

Currently is Sequenze_Fig7 used to store data at runtime for documentation.

Change the path to another new Sequenze in the DataManager.cpp at line 77. 

Do a custom fingerpose with your right hand (left hand is not implemented). Currently is a pointer pose stored which is shown in the video:
https://youtu.be/t2DlQpnEVfQ

If this pose doesnt work for you, hold your hand to be fully visible to the meta quest

Do a simple pose (as pointing, thumbs up or peace) and press 'R' on your keyboard to record. After restarting the game the changes are applied permamently.

Now if you move a green debug sphere should appear around your body while moving the hand if it is currently doing given pose. Use the video as reference.

Data is recorded while you hold the current pose until the pose is not detected anymore, e.g by opening the hand.

Make sure to face againt the big white cube with the line in the middle and draw the gesture around 150 to 250 times with some variations.

When done make sure to check if the data is stored correctly in your sequenze_fig.txt and check for blank lines and remove them 

In the Handtracker.cpp in the trainhmm() function create a new hmm at line 104.

Provide the model and data path for the new model similar to line 134 to 137

Do the same thing as in line 189 to 192 for the new model and add the new model to the hmm_models list and add the name of the model to the model_names list in the handtracker.h file at line 45.

Rebuild the project and it should run.

After starting the game it takes between 10 to 20 seconds to actually start because it trains all models with 20 epochs which leads to good results.
Loading already trained models doesnt properly work in this version and leads often to crash the engine. 




