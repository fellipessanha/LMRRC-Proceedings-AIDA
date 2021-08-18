OptFrame
========

<p align="center">
  <a href="https://github.com/optframe/optframe">
    <img src="https://tokei.rs/b1/github/optframe/optframe?category=lines" alt="Current total lines.">
  </a>
  <a href="https://github.com/optframe/optframe/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-LGPL%20v3-blue.svg" alt="License.">
  </a>
</p>

## OptFrame 4.2-dev - Optimization Framework

OptFrame v4.1 is launched (4.2 is coming)!

Now, with newest C++20 features and integrated Unit Testing for user projects and examples (still ongoing work). Scanner++ library is also dettached independent project now.
Remember to clone this repo using `--recursive` option, so you don't miss anything ;)

Major changes happened from v3 to v4, so we will need to update our tutorials...

-----

**Official Documentation on [ReadTheDocs](https://optframe.readthedocs.io)**

[![Documentation Status](https://readthedocs.org/projects/optframe/badge/?version=latest)](https://optframe.readthedocs.io/en/latest/?badge=latest)


Note that OptFrame website may have obsolete documentation and examples:

~Check [https://optframe.github.io/docs/quick-start-guide/](https://optframe.github.io/docs/quick-start-guide/)~

### Intended changes for 4.2

For 4.2 we plan to:
- migrate into MIT License model, that corresponds into newer open-source community global standards (from previous LGPLv3 license)
- fix many bugs from v4.1
- adopt standard memory management for all components (from *widget&* into *not null shared pointer* "sref")


### Docs Requirements

`python3 -m pip install -r requirements.txt`

TODO: use [exhale](https://exhale.readthedocs.io/en/latest/usage.html#usage-quickstart-guide)

Just type: `make docs`

Output is on: `xdg-open $(pwd)/docs/build/html/index.html`

## Build with Bazel

First, install [Bazel Build](https://bazel.build): `npm install -g @bazel/bazelisk`

Also, install bazel buildifier (recommended): `go get -v github.com/bazelbuild/buildtools/buildifier` (should appear on `$HOME/go/bin`) 


To build (currently a small test on `demo_opt`):
`bazel build ...`

Resulting binaries will be located at folder `./bazel-bin/demo_opt/`

To clean:
`bazel clean` or `bazel clean --expunge`


## Project Structure

Project is organized in three main folders: `src`, `libs`, `tests` and `bin`.
* On `src/OptFrame` and `tests/OptFrame` are code and testing for OptFrame framework itself.
* On `Examples` you can find code and tests for the provided Examples.
* On `MyProjects` you can find code and tests for your own codes.
* On `bin`, you can find classic `optframe-mct.sh` script, that will generate a new project `X` on `MyProjects/X/src` folder (and tests on `MyProjects/X/tests`).
* On `libs` you can find third-party libraries (including testing and benchmarking, as they are used widely on examples and subprojects)
Tests are provided via `catch2`, benchmaking via [benchmark](https://github.com/google/benchmark) library, and also `scannerpplib` (Scanner++ library) for stream/file processing.

OptFrame Functional Core (FCore) is also provided (strict `c++20`):

* On `src/OptFCore` and `Examples/FCore-Examples` are code and testing for `FCore` framework itself.


## C++ Requirements

The following requirements apply for projects (and subprojects):

* OptFrame - `c++17` - tested on `gcc-7` with flags `-fconcepts`
* FCore - `c++20` - tested on `gcc-10.1` with flags `-fcoroutines`
* Scanner++ - `c++11` - tested on `gcc-7`

To install gcc-10.1, see these [Instructions](libs/gcc-10-install/INSTRUCTIONS.md).

## Concepts General

On general two concepts are considered on Search methods (they will be better explained in next section):
- XES and XSH: XES is base type for (primary) search type XSH, which is on general words "what users want"
- XES2 and XSH2: XES2 is base type for (secondary) search/exploration type XSH2, which is "what method uses to give what users want"

Typically, XSH2=XSH=XES for trajectory-based optimization, and XSH2 is population for population-based methods. On multi-objective scenarios, XSH may be some Pareto structure, while XSH2 some "Pareto Population" (see NSGA-II, for example).


## Concepts (Intro)

A major change happened from OptFrame v3 to v4, where C++ Concepts were finally adopted (before official release of C++20 we use `concepts lite` on `gcc-7`).
This allowed a further simplification of project, by dividing most templates into two basic categories:
- Solution Space: `XS` template concept
- Objective Space: `XEv` template concept

On general, methods will work over a simplified Search Space, composed by a pair `<XS, XEv>` (or equivalent structure). Users will likely just use a pair of classical `Solution` and `Evaluation` classes, or inherit from `IESolution` to make its own personalized class ;)

Note that this will be valid for both Single- and Multi-Objective problems, unifying even more all kinds of techniques, into basic containers.

Specially for Multi-Objective problems, the standard Search Space will consist of `2^XES` space, where `XES=<XS, XEv>`. A classic member of `2^XES` is the `Pareto` class (together with a `MultiEvaluator`).

### Solution and Evaluation containers

Basic containers are still default `Solution` and `Evaluation`. Note that both are now not mandatory (thanks to concepts). It's still a very good deal to use them, as they allow templated primitive types like `int` and `double` to be directly used on `Evaluation`, also using basic std structures like `vector` directly on `Solution`.
If you don't like them, feel free to just replace them (`XS` and `XEv` can become virtually anything you want).

One can also personalize `Evaluation` class with a `MultiObjValue` tuple. This theoretically allows a unification between SingleObj and MultiObj methods, without further changes.

OptFrame 3 relied strongly on templates `R` and `ADS`, but now those only exist for basic Solution containers, and are not mandatory anymore (yet, it may still be good to follow this amazing optimization pattern).

Things are still moving, more big changes are coming.

### Search Space details

Search Space is defined on OptFrame as the pair of Solution* Space and Objective Space.
It also accepts a powerset of Solution Space together with powerset of Objective Space (for population/multiobjective search).

(*) Solution Space is also called Decision Space by some authors: `Lust, T., & Teghem, J. (2009). Two-phase Pareto local search for the biobjective traveling salesman problem. Journal of Heuristics, 16(3), 475–510. doi:10.1007/s10732-009-9103-9`.

### InitialSearch

`InitialSearch` corresponds to (OptFrame v3) `Constructive`, in the same way, Solution Space corresponds to Search Space.
Thus it can be used to generate initial valid "solution" that also comprises an objective value.

### Examples

Please take a deep look at `Examples` folder, and build them by simply typing `make` there.
Each example is connected to some previous literature work, and can demonstrate how each metaheuristic work on practice.
We know some detailed tutorial is still missing, but we hope to provide more info as soon as possible. Feel free to open issues and new discussions on GitHub ;)

#### Using Bazel

Each example is itself an independent workspace.
For this reason, each example has a reference to `@OptFrame` (as `local_repository`), 
while optframe root itself has a self reference for `@OptFrame` (as `local_repository`).

This allows building all examples from root, with `bazel build ...` or `bazel build //Examples/...`
One can also jump into each independent example and build it locally.

See available examples on bazel: `bazel query //Examples/...`

*Here are some examples and build advices for bazel.*

#### FCore-Examples

From root:
- `bazel build @FCore-Examples//...`
- `bazel run @FCore-Examples//src:app_TSPfcore`

Binary will be located at `./bazel-bin/external/FCore-Examples/src/app_TSPfcore`

From example (`cd Examples/FCore-Examples/`):
- `bazel clean`
- `bazel build ...`
- `bazel run //src:app_TSPfcore`

Binary will be located at `./Examples/FCore-Examples/bazel-bin/src/app_TSPfcore`

#### EternityII Example

From root:
- `bazel build @EternityII//...`
- `cd ./bazel-bin/external/EternityII/app_Example.runfiles/EternityII/`
- `./app_Example`

Binary and data will be located at `./bazel-bin/external/EternityII/app_Example.runfiles/EternityII/`

From example (`cd Examples/EternityII/`):
- `bazel clean`
- `bazel build ...`
- `bazel run //:app_Example`

Binary and data will be located at `./Examples/EternityII/bazel-bin/app_Example.runfiles/__main__/`

## Submodules

Getting submodules: `git submodule update --init --recursive` and `git pull --recurse-submodules`.

## Using MCT script - Make a Compilable Thing!

Just type `cd ./bin && ./mct.sh` (or `make mct`) and follow the instructions on screen.
You can find your new project on `src/MyProjects/` (and tests on `tests/MyProjects/`).

## Special thanks for SourceForge.net

The OptFrame project was hosted in SourceForge.net for 8 years and we are
very grateful for all support during this time. Thanks to this support, we
have published several academic papers and solved many large scale problems
during these years. In order to easily interact with new project collaborators
the project is now moved to GitHub platform.

Thanks a lot SourceForge/GitHub!

## A brief history of OptFrame

### OptFrame v1
* Born on 2007 at Universidade Federal de Ouro Preto, Ouro Preto/MG, Brazil
* Very early phases of project, mostly used for Open Pit Mining optimization and VNS/ILS
* Too many type casts (no templates yet!)
* No efficient version control for source files

### OptFrame v2
* Adopted templates and nice metaprogramming design
* Adopted SVN version control
* Evolution on many aspects, due to collaborations on Universidade Federal Fluminense, Niterói/RJ, Brazil
* Mostly maintained by four initial OptHouse members: Igor Machado, Sabir Ribas, Pablo Munhoz and Mário Perché

### OptFrame v3
* Stable release with many techniques for Single and Multi-objective
* Automated runtime testing with CheckCommand
* Practical implementations on several Master and PhD thesis
* Professional implementations on industry software
* Moved to Git source control
* Mostly maintained by Igor Machado and Vitor Nazário Coelho

### OptFrame v4
* Just born with C++ Concepts in mind!
* Byproduct of several worldwide collaborations, with best known software management practices
* Focusing on achieving maximum C++ performance, solving too many memory management complex strategies (fully using self managed memory by smart pointers and move semantics)
* Maximum personalization by the user


## Installation

OptFrame is organized in several C++ headers.
To install it system-wide (in linux), just type `make install` (`sudo` will be necessary here):

- headers will be put on `/usr/local/include`
- examples, src and tests will be put on `/usr/local/optframe`

We recommend to run `make test-install` after that, to ensure everything is fine!

## Examples

Interesting examples are available (see `Examples` folder):

- [EternityII](./Examples/EternityII/README.md): Eternity II Puzzle  
- [HFM](./Examples/HFM/README.md): HFM forecast framework by @vncoelho
- [HFMVRP](./Examples/HFMVRP/README.md): Heterogeneous Fleet Vehicle Routing Problem
- [KP](./Examples/KP/README.md): Knapsack Problem (v1)
- [KP2](./Examples/KP2/README.md): Knapsack Problem (v2)  
- [MITSP](./Examples/MITSP/README.md): Multi Improvement Dynamic Programming for Traveling Salesman Problem
- [MODM](./Examples/MODM/README.md): Multi Objective Direct Marketing Problem
- [OPM](./Examples/OPM/README.md): Open Pit Mining Operational Planning Problem
- [OptHS](./Examples/OptHS/README.md): OptHouse Cleaning Scheduling
- [ParallelTSP](./Examples/ParallelTSP/README.md): Parallel TSP Prototype
- [PMedCap](./Examples/PMedCap/README.md): Capacitated P-Median Problem
- [PN](./Examples/PN/README.md): Number Partition Problem
- [SVRPDSP](./Examples/SVRPDSP/README.md): Single Vehicle Routing Problem with Deliveries and Selective Pickups
- [TSP](./Examples/TSP/README.md): Traveling Salesman Problem (v1)  
- [TSP2](./Examples/TSP2/README.md): Traveling Salesman Problem (v2)  

### Building Examples

To build examples, just type `make` on `Examples` folder.

The following examples are currently prioritary, due to heavy burden for maintaining all of them:

- pmedcap 
- kp 
- tsp 
- hfvrpmt
- mitsp
- etii
- opths 
- gfp (?)
- hfm 
- modm

For more information on the others, please file an Issue.

### Metaheuristic Implementations on Examples

These examples explore several (meta)-heuristic implementations, such as:

- Simulated Annealing
- Genetic Algorithm
- Memetic Algorithm
- Variable Neighborhood Search
- Iterated Local Search
- Tabu Search
- ...

Multi-Objective metaheuristics:

- NSGA-II
- MOVNS
- 2PPLS
- ...

## VSCode Settings

We recommend the following settings for vscode (`.vscode/settings.json`):
```json
{
    "[cpp]": {
        "editor.tabSize": 3,
        "editor.detectIndentation": false
    },
    "C_Cpp.intelliSenseEngine": "Tag Parser",
    "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle : Mozilla , ColumnLimit : 0, IndentWidth: 3, AccessModifierOffset: -3}",
    "testMate.cpp.test.executables": "{tests,build,Build,BUILD,out,Out,OUT}/**/*{test,Test,TEST}*"
}
```

We recommend Microsoft `C/C++` and `C++ TestMate` extensions.

## Citation

Cite this in your paper as:

Coelho, I.M., Ribas, S., Perché, M.H.P., Munhoz, P., Souza, M.J.F., Ochi, L.S. (2010). 
OptFrame: a computational framework for combinatorial optimization problems. 
In Anais do XLII Simpósio Brasileiro de Pesquisa Operacional (SBPO). 
Bento Gonçalves-RS, pp 1887-1898.

```bibtex
@article{optframe2010,
    author = {Igor Coelho and Sabir Ribas and Mário Henrique de Paiva Perché and Pablo Munhoz and Marcone Souza and Luiz Ochi},
    year = {2010},
    month = {08},
    pages = {1887-1898},
    title = {OptFrame: a computational framework for combinatorial optimization problems},
    journal = "Simpósio Brasileiro de Pesquisa Operacional"
}
```

 
## Licence

- (v4.2+): MIT License

- (<= v4.1): [**LICENSE GNU Lesser General Public License v3**](https://github.com/optframe/optframe/blob/master/LICENSE)

*OptFrame maintainers* [@igormcoelho](https://github.com/igormcoelho) and [@vncoelho](https://github.com/vncoelho)

Copyright (C) 2007-2021

The OptFrame team
