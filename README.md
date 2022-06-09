# ConsensusPackage

Package to simulate consensus decision-making using opinion formation.

Implemented so far:\\
* Model which takes distribution of characteristics (talkativeness, persuasiveness, stubborness)
* Replicated version of the model
* Two update functions: sigmoid or step
* Continuous and discrete opinions (discrete has only two values)

To implement:\\
* Model which takes a number of leaders with the characteristics of leaders
* More update functions (ratio, linear) + bounded confidence
* Network?
* Output more adapted to dataframe/datatable (no need to transpose)
