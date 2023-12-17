#include "scheduler.hpp"

void test_scheduler(){
	ServiceConfigLocal *s1 = createService("nginx", 2);
    ServiceConfigLocal *s2 = createService("apache", 3);
    ServiceConfigLocal *s3 = createService("webapp", 1);
    ServiceConfigLocal *s4 = createService("loadbalancer", 1);

    vector<ServiceConfigLocal *> ss1 = {s1, s2};
    vector<ServiceConfigLocal *> ss2 = {s3, s4};
    ClusterConfigLocal *c1 = createCluster("us-east", ss1);
    ClusterConfigLocal *c2 = createCluster("us-west", ss2);

    vector<ClusterConfigLocal *> cs = {c1, c2};
    RegionConfigLocal *r1 = createRegion("us", cs);

    vector<RegionConfigLocal *> rs = {r1};
    GlobalConfigLocal *currGlobalConfigLocal = createGlobal(rs);

    cout << "Before updates:" << endl;
    printGlobalConfigLocal(currGlobalConfigLocal);
    cout << endl;

    pair<string, unordered_map<string, int>> update;
    update.first = "us";

    unordered_map<string, int> service_map;
    service_map["nnginx"] = 4;
    update.second = service_map;

    scheduler(currGlobalConfigLocal, update);

    cout << "After updates:" << endl;
    printGlobalConfigLocal(currGlobalConfigLocal);
}

void test_updater(){
    ServiceConfigLocal *s1 = createService("server", 2);
    ServiceConfigLocal *s2 = createService("client", 1);

    vector<ServiceConfigLocal *> ss1 = {s1};
    ClusterConfigLocal *c1 = createCluster("us-east", ss1);

    vector<ClusterConfigLocal *> cs = {c1};
    RegionConfigLocal *r1 = createRegion("us", cs);

    vector<RegionConfigLocal *> rs = {r1};
    GlobalConfigLocal *currGlobalConfigLocal = createGlobal(rs);

	updateLocal("us", "us-east", currGlobalConfigLocal);
}


