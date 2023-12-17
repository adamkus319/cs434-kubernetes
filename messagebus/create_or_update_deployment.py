import argparse
from kubernetes import client, config

def deployment_exists(api_instance, deployment_name, namespace):
    '''Determine if deployment exists in cluster'''
    try:
        api_instance.read_namespaced_deployment(deployment_name, namespace)
        return True
    except client.ApiException as e:
        if e.status == 404:
            return False
        else:
            print(f"Exception when calling AppsV1Api->read_namespaced_deployment: {e}")
            return False

def update_replicas(api_instance, deployment_name, namespace, replicas):
    try:
        deployment = api_instance.read_namespaced_deployment(deployment_name, namespace)

        deployment.spec.replicas = replicas  # update number of replicas

        api_instance.patch_namespaced_deployment(
            name=deployment_name,
            namespace=namespace,
            body=deployment
        )

        print(f"Deployment {deployment_name} updated successfully")

    except client.ApiException as e:
        print(f"Exception when calling AppsV1Api->patch_namespaced_deployment: {e}")

def create_deployment(api_instance, deployment_name, namespace, replicas):
    service_name = deployment_name[:-len("_deployment")]  # remove _deployment
    deployment = client.V1Deployment(
        api_version="apps/v1",
        kind="Deployment",
        metadata=client.V1ObjectMeta(name=deployment_name),
        spec=client.V1DeploymentSpec(
            replicas=replicas,
            selector=client.V1LabelSelector(match_labels={"app": deployment_name}),
            template=client.V1PodTemplateSpec(
                metadata=client.V1ObjectMeta(labels={"app": deployment_name}),
                spec=client.V1PodSpec(
                    containers=[
                        client.V1Container(
                            name=deployment_name,
                            image=service_name + ":latest",
                            ports=[client.V1ContainerPort(container_port=8080)]
                        )
                    ]
                )
            )
        )
    )

    try:
        api_instance.create_namespaced_deployment(namespace, deployment)  # create new deployment

        print(f"Deployment {deployment_name} created successfully")

    except client.ApiException as e:
        print(f"Exception when calling AppsV1Api->create_namespaced_deployment: {e}")


def create_or_update_deployment(deployment_name, namespace, replicas):
    config.load_kube_config()

    api_instance = client.AppsV1Api()  # create instance of API client

    if deployment_exists(api_instance, deployment_name, namespace):
        update_replicas(api_instance, deployment_name, namespace, replicas)  # update replicas if deployment exists
    else:
        create_deployment(api_instance, deployment_name, namespace, replicas)  # create new deployment if deployment doesn't exist


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create or update a Kubernetes deployment")

    parser.add_argument("deployment_name", type=str, help="Name of deployment")
    parser.add_argument("namespace", type=str, help="Namespace of the deployment")
    parser.add_argument("replicas", type=int, help="Number of replicas")

    args = parser.parse_args()

    create_or_update_deployment(args.deployment_name, args.namespace, args.replicas)

    


