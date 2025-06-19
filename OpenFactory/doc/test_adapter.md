## Testing an OpenFactory Adapter

To test an OpenFactory adapter, create a folder (e.g., `my_adapter`) that contains:

* The MTConnect device model file (e.g., `my_device.xml`) for the device your adapter is developed for.
* The OpenFactory device deployment file (e.g., `my_device.yml`).

A typical OpenFactory device deployment file looks like this:

```yaml
devices:
  my-device:
    uuid: <device-uuid>

    agent:
      port: <agent-port>
      device_xml: <device-xml>
      adapter:
        ip: <adapter-ip>
        port: <adapter-port>
```
The `agent-port` can be freely chosen and will be exposed for accessing the MTConnect agent.
The `<device-xml>` is the file with the MTConnect device model.

### Deploying the Device

To deploy the device, run:

```bash
openfactory-sdk device up path/to/my_device.yml
```

This will start a containerized MTConnect agent that:

* Listens on the specified `agent-port`
* Connects to your adapter at the given `adapter-ip` and `adapter-port`
* Streams data from the agent to OpenFactory via a containerized Kafka producer

### Verifying Adapter Output

To verify your adapter is working correctly, you can either:

* Check the Kafka producer container logs, or
* Inspect the data in Kafka using `ksql`.

To use `ksql`, run:
```bash
ksql
```
Then, in the KSQL CLI:
```sql
SELECT * FROM ASSETS WHERE ASSET_UUID = '<device-uuid>' EMIT CHANGES;
```
This query will continuously show the data streamed by your adapter to Kafka.
