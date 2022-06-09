
#Example with everything in output
consensus_model("All",c(10,rep(1,9)),rep(1,10),rep(1,10),1,10,"time","step",max_update=0.1,seed=2.417610e-09)
consensus_model("All",c(10,rep(1,9)),rep(1,10),rep(1,10),1,0.05,"consensus","step",max_update=0.1,seed=2.417610e-09)

consensus_model("All",c(10,rep(1,9)),rep(1,10),rep(1,10),1,10,"time","step",max_update=0.1,seed=2, type_opinions = "discrete")
consensus_model("All",c(100,rep(1,19)),rep(1,20),rep(1,20),2,0.,"consensus","step",
                max_update=1,seed=runif(1)*100, type_opinions = "discrete", max_time = 5000)

#Example of replicated version
replicate_consensus_model(5,"Simulation",c(1,rep(1,9)),rep(1,10),rep(1,10),1,1,"time","sigmoid",max_update=0.01)


library(data.table)
library(ggplot2)
#Plot time
res=replicate_consensus_model(1000,"Simulation",c(1,rep(1,9)),rep(1,10),rep(1,10),1,0.02,"consensus","step",max_update=0.1)
res=replicate_consensus_model(1000,"Simulation",c(1,rep(1,9)),rep(1,10),rep(1,10),1,0.02,"consensus","step",max_update=1,type_opinions="discrete")

dt=as.data.table(transpose(res))
setnames(dt,c("seed","time","mean_x","std_x"))

ggplot(dt,aes(y=time))+
  geom_boxplot()

#Vary distribution and plot time as fct of n leaders.
#Could be implemented directly in C++
n_pop = 50
n_simul = 500
dt = data.frame()
for(i in 0:n_pop){
  
  res=replicate_consensus_model(n_simul,"Simulation",c(rep(100,i),rep(1,n_pop-i)),c(rep(100,i),rep(1,n_pop-i)),c(rep(100,i),rep(1,n_pop-i)),1,0.,"consensus","step",max_update=1, type_opinions="discrete")
  dt=rbind(dt,as.data.table(transpose(res)))
}
setnames(dt,c("seed","time","mean_x","std_x"))
dt[,n_leader := rep(seq(0,n_pop),each=n_simul)]

ggplot(dt,aes(group=n_leader,y=time))+
  geom_boxplot()

dt_summary = dt[,.(mean_time = mean(time),std_time = sd(time)),by=n_leader]
ggplot(dt_summary,aes(x=n_leader,y=mean_time))+
  geom_ribbon(aes(ymin = mean_time - std_time, ymax = mean_time + std_time),alpha=0.5)+
  geom_point()+
  geom_line()
  


