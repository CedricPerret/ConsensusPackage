
consensus_model("All",c(10,rep(1,9)),rep(1,10),rep(1,10),1,10,"time","step",10,max_update=0.1,seed=2.417610e-09)

replicate_consensus_model(5,"Simulation",c(1,rep(1,9)),rep(1,10),rep(1,10),1,1,"time","sigmoid",10,max_update=0.01)


library(data.table)
library(ggplot2)
#Make a version with number of leader/followers
res=replicate_consensus_model(1000,"Simulation",c(1,rep(1,9)),rep(1,10),rep(1,10),1,0.02,"consensus","step",10,max_update=0.01)

dt=as.data.table(transpose(res))
setnames(dt,c("Seed","Time","Mean","Std"))

ggplot(dt,aes(y=Time))+
  geom_boxplot()






