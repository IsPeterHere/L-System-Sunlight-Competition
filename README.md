# L-System Sunlight Competition

## Telegraphic rundown

*what?* - A Simulation in which "plants" compete against each other for "sunlight".  

*how?* - The plants "growth patten" is encoded by an L-system rule-set which indicates how the plant should grow its superstructure of "sticks" and "leafs". (Each Successive plant will have some nominal variation from the parent plants rule-set). The competition emerges as the key recourse "Sunlight" is collected from only from the highest "leaf" (i.e. in any vertical line only the "plant" with the highest "leaf" will receive "sunlight")

*why?* - Different successful growth patterns will emerge depending on the conditions chosen at start (conditions are set in constants in LSSC.ccp).

## Some Common structures
<p align="center">
  <img src="https://github.com/IsPeterHere/L-System-Sunlight-Competition/blob/main/Screenshots/SmallSparse.png" width="800" alt="Tap O' Noth">
  
  Early on different plants will emerge but the effects of competition will not appear until there are enough successful plants and they dense enough that availability of sunlight becomes the primary constraint.
</p>


<p align="center" >
  <img src="https://github.com/IsPeterHere/L-System-Sunlight-Competition/blob/main/Screenshots/TallGrasses.png" width="800" alt="Tap O' Noth">

  In many cases like above wavy "grass" like plants will dominate. The main interesting effect that can be observed for this sort is the tendency for them to try and curl while still maximising height.
</p>

<p align="center">
  <img src="https://github.com/IsPeterHere/L-System-Sunlight-Competition/blob/main/Screenshots/competitvePlants.png" width="800" alt="Tap O' Noth">

  Though in many other cases (especially when "propagation" depends on getting a lot of sunlight and not just some) branching structures will dominate and compete.
</p>

## Other resources used

<a href = "https://github.com/IsPeterHere/Computational-Models">Computational Models</a> - *L-System taken from here*  
<a href = "https://github.com/IsPeterHere/Vulkan-Interface">Vulkan Interface</a> - *Used for the display*
