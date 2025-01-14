# PQ-VEINS

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10189557.svg)](https://doi.org/10.5281/zenodo.10189557)

*PQ-VEINS is an extension of VEINS, whose README is included here for attribution*

> Veins - The open source vehicular network simulation framework.
>
> See the Veins website <http://veins.car2x.org/> for a tutorial, documentation,
and publications.
>
>Veins is composed of many parts. See the version control log for a full list of
>contributors and modifications. Each part is protected by its own, individual
>copyright(s), but can be redistributed and/or modified under an open source
>license. License terms are available at the top of each file. Parts that do not
>explicitly include license text shall be assumed to be governed by the "GNU
>General Public License" as published by the Free Software Foundation -- either
>version 2 of the License, or (at your option) any later version
>(SPDX-License-Identifier: GPL-2.0-or-later). Parts that are not source code and
>do not include license text shall be assumed to allow the Creative Commons
>"Attribution-ShareAlike 4.0 International License" as an additional option
>(SPDX-License-Identifier: GPL-2.0-or-later OR CC-BY-SA-4.0). Full license texts
>are available with the source distribution.

This repository is made available as part of an artifact submission to the 2024 Network and Distributed 
System Security Symposium (NDSS), associated with the following paper:

Geoff Twardokus, Nina Bindel, Hanif Rahbari, and Sarah McCarthy, "When Cryptography Needs a Hand: Practical Post-Quantum Authentication for V2V Communications," _Network and Distributed System Security Symposium (NDSS 2024)_, San Diego, CA, Feb. 2024.

Bibtex:

    @inproceedings{twardokus2024when,
        author = "Geoff Twardokus and Nina Bindel and Hanif Rahbari and Sarah McCarthy",
        title = "When Cryptography Needs a Hand: Practical Post-Quantum Authentication for {V2V} Communications",
        booktitle = "Proc. Network and Distributed System Security Symposium (NDSS)",
        month = feb,
        year = "2024",
        address = "San Diego, CA",
        pages = ""
    }

## Repository Structure

PQ-VEINS is an extension of VEINS (see above). This implementation is based on Instant VEINS version 5.2, 
which is a release of VEINS packaged in a virtual machine for easy distribution and portability. 

This repository has a simple structure. Documentation can be found in `doc`, example scenario configuration files
can be found in `examples`, and all source code is in `src`. Describing the entire existing VEINS project is beyond our 
scope; however, specific changes/additions made to implement PQ-VEINS are enumerated as follows:
- The PQ-V2V protocol described in our NDSS paper is implemented as an application, PQV2VApp, that is run by 
simulated vehicles. The implementation files for PQV2VApp are in `src/veins/modules/application/traci`.
- Source files to implement V2V messages required for PQC support (including digitally signed messages, P2PCD learning
requests and responses, etc.) can be found in `src/veins/modules/messages`.
- Example scenario configurations for various instantiations of the _Partially Hybrid_ protocol from our NDSS paper
can be found in `examples/omnetpp_ini` (e.g., the parameters to run ECDSA-secured V2V using a data rate of 6 Mbps and
no peer-to-peer certificate sharing can be found under `[Config V2V-ECDSA-BPSK12-NOP2PCD-500MS]` in that file).

Complete documentation for VEINS beyond the above modifications made to implement PQ-VEINS can be found on the main
VEINS project, available on GitHub at https://github.com/sommer/instant-veins/tree/instant-veins-5.2-i1. 

## Minimum Working Example
A Debian Linux virtual machine that is pre-configured to compile and run the artifact is archived in OVA format
at [doi:10.5281/zenodo.10160535](https://doi.org/10.5281/zenodo.10160535). This virtual machine was evaluated using Virtualbox version 7.0.8 r156879 (Qt5.15.2), which is freely available from Oracle at
https://www.virtualbox.org. We recommend users and evaluators use the same version to ensure consistency.

## Evaluation Requirements and Supported Environments

### Supported operating system
PQ-VEINS was built and tested using Instant Veins, which is based on Debian Linux. VEINS, and hence PQ-VEINS, _should_
be able to run on any mainstream PC, Mac, or Linux operating system, but no testing or guarantee is given for operating
systems aside from Instant Veins and its associated Debian OS.

### Software
We have built and evaluated our artifacts using [Instant VEINS](https://veins.car2x.org/documentation/instant-veins/),
a virtual machine-based version of the [VEINS](https://veins.car2x.org) simulator
tool that comes pre-packaged with all tools and dependencies installed in a Debian Linux instance. Due to the complexity
of installing and configuring multiple components (OmNET++, SUMO, INET Framework, SimuLTE, etc.) that all have to work
together in order to operate VEINS, we strongly encourage anyone interested in PQ-VEINS to start with the virtual
machine provided above (see
Minumum Working Example) and proceed from there.

However, VEINS can also be installed from source if desired - users with
a specific need that requires this approach should refer to the
[VEINS installation instructions](https://veins.car2x.org/tutorial/) and configure the VEINS
environment and its dependencies, and can then install PQ-VEINS by copying two directories from our archived repository
into the freshly installed environment (see detailed instructions below).

No software is required beyond what is pre-installed with Instant VEINS and/or required to install VEINS from source as
per the linked instructions above from the VEINS project.

### Hardware
No hardware is required beyond a "commodity PC." As defined by the 
[NDSS 2024 Call for Artifacts](https://web.archive.org/web/20230930035437/https://secartifacts.github.io/ndss2024/call), 
"[a] commodity desktop machine is defined as one with an x86-64 CPU with 8 cores and 16 GB of RAM running a recent Linux 
or Windows operating system."

## Installing PQ-VEINS

PQ-VEINS is based on Instant VEINS, a virtual machine-based release of the VEINS 
simulator.
While PQ-VEINS can be integrated with any existing VEINS installation, we recommend
using our pre-configured PQ-VEINS VM, which is available [here](https://bit.ly/3PWTHNN).

A more complete guide to installing PQ-VEINS in an existing VEINS environment 
is forthcoming. 

## Design of PQ-VEINS

In order to support both classical and post-quantum V2V security, which are not modeled 
in the basic VEINS tool, a number of additions and extensions have been made to VEINS.
For example, new V2V messages are defined for digitally signed SPDUs under

    pq-veins/src/veins/modules/messages/

These messages (and other additions) are defined within the source code of VEINS to
allow for easy integration into customized scenarios and applications. We include
a demonstration application, "PQV2VApp", whose source code can be found in
`src/veins/modules/application/traci/PQV2VApp.cc` in this repository as well as
in the demonstration VM linked above. 

This application adds support for V2V security (using either Falcon for PQC, or the current
ECDSA standard under IEEE 1609.2), as well as the peer-to-peer certificate distribution protocol 
(P2PCD) as defined in IEEE 1609.2.
Depending on the choice of algorithm, which can be configured in the runtime settings for the 
PQV2VApp application, the sizes of simulated messages are adjusted to match what would occur
if the selected algorithm was used for digitally signing messages and certificates.

## Running PQ-VEINS Scenarios

A variety of pre-configured demonstration scenarios can be run using PQ-VEINS in the 
virtual machine linked above. The various runtime configurations are enumerated in 

    veins/examples/veins/omnetpp.ini

_Note that our provided config file enumerates the runtime configurations we consider to be
part of this artifact, but `omnetpp.ini` can be further customized as desired (see 
the [OmNET++ manual](https://doc.omnetpp.org/omnetpp/manual/) for guidance on syntax and 
options for formatting this file)._

To run simulations using PQ-VEINS, begin by launching the required background processes.
Open a terminal and use the command

    veins_launchd -vv

Then, in a new terminal, navigate to the PQ-VEINS application directory:

    cd /home/veins/src/veins/examples/veins

Here, the `run` utility can be used to execute any configuration described in `omnetpp.ini`.
For example,

    ./run -u Cmdenv -c V2V-Falcon-BPSK12-WITHP2PCD-500MS

will execute a scenario with Falcon used for message signing and verification, DSRC communication
set to a data rate that corresponds to BPSK 1/2, P2PCD enabled, and the typical 500ms gap between
full-certificate SPDU transmissions by a given vehicle.

## Analyzing PQ-VEINS Results

Upon completion of a run, the results will be saved in scalar vector format within the `results`
directory (`/home/veins/src/veins/examples/veins/results/`). We provide a script to quickly 
convert the results from an unrestricted number of simulation iterations into CSV format:

      python3 ./process_results.py

Results stored in the CSV files will then be accessible within the `results` folder and can
be post-processed easily in whatever manner one may desire.
