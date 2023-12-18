#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include "configHelper.hpp"

using namespace std;

/* SCHEDULING FUNCTIONS */
int clusterServices(string target_service, ClusterConfigLocal cluster);
void updateCluster(string target_service, string op, int &services_left, ClusterConfigLocal *cluster);
void updateRegion(string target_service, string op, int &services_left, RegionConfigLocal *region);
void scheduler(GlobalConfigLocal *currGlobalConfigLocal, pair<string, unordered_map<string, int>> update);

/* K8S DEPLOYMENT FUNCTIONS */
void updateDeploymentsLocal(vector<ServiceConfigLocal *> services);
void updateLocal(string regionName, string clusterName, GlobalConfigLocal *currGlobalConfigLocal);

/* TEST FUNCTIONS */
void test_scheduler();
void test_updater();
