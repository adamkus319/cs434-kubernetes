#include "scheduler.hpp"

// update deployments based on services
void updateDeploymentsLocal(vector<ServiceConfigLocal *> services){
	string namespaceName = "default";  // default namespace value

	vector<string> deploymentVector;  // vector of deployment names

	// loop through services in updated config, adding them or changing number of copies
	for(const auto& service_ptr : services){
		string serviceName = service_ptr->serviceName;
		string deploymentName = serviceName + "-deployment";
		int desiredCount = service_ptr->count;

		deploymentVector.push_back(deploymentName);  // keep track of deployments

		string pythonScript = "create_or_update_deployment.py";

		string command = "python3 " + pythonScript + " " +
			deploymentName + " " + namespaceName + " " +
			to_string(desiredCount);

		// call script to create/update deployment
		int result = system(command.c_str());

		if(result != 0){
			cout << "Failed to update deployment " << deploymentName << endl;
			return;
		}
		else{
			cout << "Updated deployment " << deploymentName << endl;
		}

	}

	ostringstream py_in;
	for(const auto& deploymentName : deploymentVector){  // parse names into stream
		py_in << deploymentName << ' ';
	}	

	string pythonScript = "remove_deployments.py"; 
	string command = "python3 " + pythonScript + " " + 
		namespaceName + " " + py_in.str();	

	int result = system(command.c_str());

	if(result != 0){
		cout << "Failed to remove old deployments" << endl;
	}
	else{
		cout << "Successfully updated deployments" << endl;
	}

	return;
}

// update k8s cluster deployments based on global config
void updateLocal(string regionName, string clusterName, GlobalConfigLocal *currGlobalConfigLocal){
	for(const auto& region_ptr : currGlobalConfigLocal->regions) {
		if(region_ptr->regionName == regionName){  // found region of host
			for(const auto& cluster_ptr : region_ptr->clusters){
				if(cluster_ptr->clusterName == clusterName){  // found cluster of host
					updateDeploymentsLocal(cluster_ptr->services);  // update deployments based on service configuration
				}
			}
		}
	}

	return;
}
