#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <numeric>
#include <time.h>
#include <Rcpp.h>
//using namespace Rcpp;

//Useful C++ code
//for (const float elem : list) {
//    std::cout << elem << std::endl;
//}

//Discrete = max update 1 et step.

void update_function(const float speaker, float &listener, 
                     const float persuasiveness_speaker, const float stubborness_listener, 
                     const std::string type_update, const float max_update, const float inc_update,
                     std::string type_opinions, std::default_random_engine generator){
    float update;
    if (type_update == "sigmoid"){
        update = max_update / (1+exp(-inc_update * (persuasiveness_speaker - stubborness_listener))) ;
    } else if (type_update == "step"){
        if (persuasiveness_speaker >= stubborness_listener){
            update = max_update;
        } else{
            update = 0;
        }
    }
    //std::cout << listener << std::endl;
    if (type_opinions == "continuous"){
      listener = listener + update * (speaker - listener);
    }else if (type_opinions == "discrete"){
      std::uniform_real_distribution<> dist(0, 1);
      if (dist(generator) < update){
        listener = speaker;
      }
    }

}


std::vector<float> initialize_opinions(const int n_individuals, std::string type_opinions, const float lower_bound, const float higher_bound,std::default_random_engine generator){
  std::vector<float> opinions;
  
  if(type_opinions == "continuous"){
    std::uniform_real_distribution<> dist(lower_bound, higher_bound);
    
    for (int i = 0; i<n_individuals; i++){
      opinions.push_back(dist(generator));
    }
  } else if (type_opinions == "discrete"){
    std::uniform_int_distribution<> dist(lower_bound, higher_bound);
    
    for (int i = 0; i<n_individuals; i++){
      opinions.push_back(float(dist(generator)));
    }
  }
  
  
  return(opinions);
}


//Make a version that can take a vector of opinions

// [[Rcpp::export]]
std::vector<std::vector<float>> consensus_model( std::string level_of_detail,
                               std::vector<float> talkativeness, std::vector<float> persuasiveness, std::vector<float> stubborness,
                               const int n_listeners, 
                               const float breaking_point, const std::string type_stop, 
                               std::string type_update,
                               int n_individuals, std::string type_opinions = "continuous", const float lower_bound = 0, const float higher_bound = 1,
                               const float max_update = 1, const float inc_update = 1, float seed = 0){
    
    
    if(seed == 0){
      seed = time(NULL);
    }
    
    std::default_random_engine generator(seed);
    std::vector<float> opinions = initialize_opinions(n_individuals,type_opinions,lower_bound,higher_bound, generator);

    

    float mean_x;
    float std_x;
    bool breaking_cond = false;
    int time_step = 0;
    
    //Initialise Output
    std::vector<std::vector<float>> res;

    
    //Proba of being a speaker (need to divide by max and add k)
    //Or use a normalised technique already
    std::discrete_distribution<> proba_speaker(talkativeness.begin(),talkativeness.end());
    
    while (breaking_cond == false) {
        
        //Choose speaker
        int speaker_index = proba_speaker(generator);
        
        //Choose listener randomly except speaker
        //Could do random if one individual rather than shuffle the whole pop
        //Tested, it works
        std::vector<int> list_listeners(0, n_individuals - 1);
        for(int i = 0; i<n_individuals; ++i){
            if (i != speaker_index){
                list_listeners.push_back(i);
            }
        }
        std::shuffle(list_listeners.begin(),list_listeners.end(),generator);


        //Calculate mean and variance
        mean_x = std::accumulate(opinions.begin(),opinions.end(),0.0) / opinions.size();
        std_x = 0;
        for(int i = 0; i<opinions.size(); ++i){
            std_x += pow(opinions[i] - mean_x,2) / (opinions.size() - 1) ;
        }
        //Write results
        if (level_of_detail == "All"){
            res.push_back(opinions);
        }else if(level_of_detail == "Time step"){
          std::vector<float> res_single_t = {time_step,mean_x,std_x} ;
          res.push_back(res_single_t);
        }
        
        
        //Choose when to stop (either as fct of time or when std is lower than a limit)
        if (type_stop == "time"){
            if (time_step >= breaking_point){
                breaking_cond = true;
            }
        } else if (type_stop == "consensus"){
            if (std_x <= breaking_point){
                breaking_cond = true;
            }
        }

        for(int i = 0; i<n_listeners; ++i){
            update_function(opinions[speaker_index], opinions[list_listeners[i]], 
                            persuasiveness[speaker_index], stubborness[list_listeners[i]], type_update, max_update, inc_update, type_opinions, generator);
        }    
        ++time_step;
    }


    
    //return Rcpp::List::create(Rcpp::Named("vec") = list);
    //return list;
    if (level_of_detail == "Simulation"){
      std::vector<float> res_single_t = {seed,time_step,mean_x,std_x} ;
      res.push_back(res_single_t);
    }
    return res; 
}


//Can directly have dataframe as output
// Rcpp::DataFrame R_create_list(std::vector<float> list, std::string level_of_detail,


// [[Rcpp::export]]
/**
 * Replicated version of consensus-decision making.
 * 
 * @param n_simul Number of replicates
 * @param n_pop Group size
 * @param 
 */

  
std::vector<std::vector<float>> replicate_consensus_model(const int n_simul, std::string level_of_detail,
                                                      std::vector<float> talkativeness, std::vector<float> persuasiveness, std::vector<float> stubborness,
                                                      const int n_listeners, 
                                                      const float breaking_point, const std::string type_stop, 
                                                      std::string type_update,
                                                      int n_individuals, std::string type_opinions = "continuous", const float lower_bound = 0, const float higher_bound = 1,
                                                      const float max_update = 1, const float inc_update = 1){

//Initialise output and seed
  std::vector<std::vector<float>> res;
  float seed;
  //Run replicates
  for(float i = 0; i<n_simul; ++i){
    seed = time(NULL)%1000000 * (i+1) ;
    res.push_back(consensus_model(level_of_detail,
                              talkativeness, persuasiveness, stubborness,
                              n_listeners,
                              breaking_point, type_stop,
                              type_update, n_individuals, type_opinions, lower_bound, higher_bound,
                              max_update, inc_update, seed)[0]);
  }
  return(res);
}





//For one element by time step
//DataFrame df = DataFrame::create(v1, v2);
//https://teuder.github.io/rcpp4everyone_en/140_dataframe.html



