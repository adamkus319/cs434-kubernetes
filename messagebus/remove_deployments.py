import sys
from kubernetes import client, config

def remove_deployment(api_instance, deployment_name, namespace):

    try:
        api_instance.delete_namespaced_deployment(  # delete deployment
            name=deployment_name,
            namespace=namespace,
            body=client.V1DeleteOptions(),
        )

        print(f"Deployment {deployment_name} deleted successfully")

    except client.ApiException as e:
        print(f"Exception when calling AppsV1Api->delete_namespaced_deployment: {e}")


if __name__ == "__main__":
    namespace = sys.argv[1]  # first CLI arg is namespace

    if len(sys.argv) > 2:  # possible deployments to remove
        updated_deployments = sys.argv[2:]  # rest of CLI args are deployments to remove

        config.load_kube_config()

        api_instance = client.AppsV1Api()

        try:
            old_deployments = api_instance.list_namespaced_deployment(namespace=namespace)

            for old_deployment in old_deployments.items:
                old_deployment_name = old_deployment.metadata.name
                if old_deployment_name not in updated_deployments:  # update if deployment is running that does not match new state
                    remove_deployment(api_instance, old_deployment_name, namespace)  # remove deployment from cluster
                    print(f"Removed deployment {old_deployment_name}")

        except client.ApiException as e:
            print(f"Exception when calling AppsV1Api->list_namespaced_deployment: {e}")


            
    else:
        print("No deployments can be removed")

    
