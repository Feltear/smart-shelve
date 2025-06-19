# OpenFactory-SDK

**OpenFactory Software Development Kit**

The **OpenFactory-SDK** provides tools to develop and test [OpenFactory](https://github.com/Demo-Smart-Factory-Concordia-University/OpenFactory) applications in a simplified development environment. Instead of requiring a full OpenFactory and Kafka cluster setup, this SDK uses lightweight Docker containers to simulate the necessary infrastructure.

---

## 🐳 Deploy OpenFactory-SDK in a Dev Container

The SDK includes a Dev Container **Feature** that automatically sets up the OpenFactory infrastructure inside your development environment.

### ✨ Usage

Add the following to your `.devcontainer/devcontainer.json`:

```json
{
  "features": {
    "docker-in-docker": {
      "version": "latest"
    },
    "ghcr.io/demo-smart-factory-concordia-university/openfactory-sdk/infra:latest": {
      "openfactory-version": "main"
    }
  }
}
```

| Options Id           | Description                                                | Type   | Default Value |
|----------------------|------------------------------------------------------------|--------|----------------|
| openfactory-version  | Git ref (branch, tag, or commit) of OpenFactory to install | string | main           |



This will:

* Install OpenFactory with the desired version

* Install the OpenFactory SDK

* Copy the OpenFactory infrastructure files into your dev container (under `/usr/local/share/openfactory-sdk/openfactory-infra`)
* Define these environment variables:
  ```
  KAFKA_BROKER=broker:9092
  KSQLDB_URL=http://ksqldb-server:8088
  ```

* Add these shell aliases:
  ```
  ksql      – launch the ksqlDB CLI
  spinup    – start infrastructure with Docker Compose
  teardown  – tear down the infrastructure
  ```

  The environment variables and aliases are available in every Bash terminal inside your dev container.

* Add the `openfactory-sdk` command

  This command allows to deploy OpenFactory assets (devices, applications) on the development OpenFactory infrastructure. Help about available commands can be obtained with
  ```
  openfactory-sdk --help
  ```

## Use cases

* [Test an OpenFactory adapter](doc/test_adapter.md)
