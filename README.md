[![Build Status](http://jenkins.sonata-nfv.eu/buildStatus/icon?job=son-examples)](http://jenkins.sonata-nfv.eu/job/son-examples)

# son-examples

This repository contains several example network services and their corresponding VNFs that can be used to test SONATA's SDK tools and SONATA's service platform.

## Services (`service-projects/`)

Example service projects that include NSDs, VNFDs, etc. The projects are given as source code and have to be packed before they can be pushed to the service platform or the emulator.

* `sonata-empty-service-emu`: Simple service chain with three empty VNFs.
* `sonata-snort-service-emu`: Service with a single Snort VNF.
* `sonata-sdk-test-service-emu`: Service with 2 empty ubuntu VNFs chained to each other.
* `sonata-ovs-service-emu`: Service with an ovs switch and a Ryu openflow controller.

### Naming scheme

* `*-emu`: Service references Docker-based VNFs that can be executed in [`son-emu`](https://github.com/sonata-nfv/son-emu).
* `*-sp`: Service references VM-based VNFs that can be executed in SONATA's service platform.

### Packaging

The given example projects can be packed with the [`son-cli`](https://github.com/sonata-nfv/son-cli) tools:

First initialize a local SONATA workspace (if not already done):

* `son-workspace --init`

In `service-projects/` do:

* `son-package --project sonata-empty-service-emu -n sonata-empty-service`
* `son-package --project sonata-snort-service-emu -n sonata-snort-service`

The created service packages can be found in:

* `<service-folder>/target/<package-file.son>`


## VNFs (`vnfs/`)

Example VNF implementations. Most of them are given as Docker container configurations so that they can directly be used in the emulator. But the Docker containers can also be included and executed within VM images to run them on the service platform.

* `sonata-empty-vnf-docker`: Minimalistic empty VNF (UPB).
* `sonata-iperf3-vnf-docker`: Iperf3 traffic generator (UPB).
* `sonata-snort-ids-vnf-docker`: Snort 2.9 intrusion detection system (UPB).
* `sonata-ovs1-vnf-docker`: OpenvSwitch inside Docker container (Packets are processed in userspace inside the vswitch daemon running in the container) (iMinds).
* `sonata-ryu-vnf-docker`: Ryu Openflow controller, running as a learning switch (Snort support to be included) (iMinds).
* `sonata-vtc-vnf-docker`: nDPI based traffic classifier (requieres PF_RING to be installed on the host machine) (NCSRD).

### Naming scheme

* `*-docker`: VNF executed as Docker container (Dockerfile).
* `*-vm`: VNF executed as virtual machine image (or Vagrantfile).

### Build & Push Docker-based VNFs

In `vnfs/` do ...

```
./build.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]

./push.sh [opt. target repository name (default=registry.sonata-nfv.eu:5000)]
```

## Usage

### Deploy example service on [`son-emu`](https://github.com/sonata-nfv/son-emu)

Step-by-step instructions that show how to deploy the example services on `son-emu` are given in `son-emu`'s project wiki:

* `sonata-empty-service:` [Service package deployment and validation](https://github.com/sonata-nfv/son-emu/wiki/Example-2)
* `sonata-snort-service:` [Snort IDS service package deployment and validation](https://github.com/sonata-nfv/son-emu/wiki/Example-3)

### Deploy example service on SONATA's service platform

* TODO (an example service for the service platform will be available soon)

## CI/CD Integration

This repository is integrated into SONATA's CI/CD workflow. The following unittests are executed whenever a pull request is created:

* Build VNFs: Builds all Docker-based VNF images (`vnfs/build.sh`).
* Push VNFs: Pushes all VNF images to SONATA's Docker registry (`vnfs/push.sh`).
* Package services: Uses `son-package` to validate the service projects (`service-projects/pack.sh`).

Note: The packaged services are not yet uploaded anywhere. We use packaging only to validate the service projects and their descriptors.

## License

Son-examples is published under Apache 2.0 license. Please see the LICENSE file for more details.

## Useful Links

* [Snort IDS](https://www.snort.org)

## Contributing
Contributing to the son-examples is really easy. You must:

1. Clone [this repository](http://github.com/sonata-nfv/son-examples);
2. Work on your proposed changes, preferably through submitting [issues](https://github.com/sonata-nfv/son-examples/issues);
3. Submit a Pull Request;
4. Follow/answer related [issues](https://github.com/sonata-nfv/son-examples/issues) (see Feedback-Chanel, below).

---
#### Lead Developers

The following lead developers are responsible for this repository and have admin rights. They can, for example, merge pull requests.

* Manuel Peuster (https://github.com/mpeuster)
* Felipe Vicens (https://github.com/felipevicens)


#### Feedback-Chanel

* You may use the mailing list [sonata-dev@lists.atosresearch.eu](mailto:sonata-dev@lists.atosresearch.eu)
* [GitHub issues](https://github.com/sonata-nfv/son-examples/issues)

