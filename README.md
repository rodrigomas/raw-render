# Multithread Pathtracer (raw-render)

Multithread Pathtracer - Used as ground test render for the DSc. Thesis

## Getting Started

The photorealistic rendering process for cinema and TV increasingly demands processing power, requiring fast parallel algorithms and effective task distribution systems. However, the processes currently used by the academia and by the industry still consume several days to evaluate an animation in super-resolution (typically 8K), what makes difficult the improvement of artistic quality and limits the number of experiments. 

In this work, we focus on the optimization of three processes involved in photorealistic rendering, reducing the total time of rendering substantially. Firstly, we optimize the local rendering, in which the system works to rendera set of pixels optimally, taking advantage of the available hardware resources and using previous rendering data. Secondly, we optimize themanagement process, which is changed not only to distribute frames but also to analyze all the rendering infrastructure, optimizing the distribution process and allowing the establishment of goals as time and cost.

Furthermore, the management model is expanded to the cloud, using the cloud as a processing overflow. Thirdly, we propose a new optimized process to evaluate the rendering task collaboratively, where each node communicates partial results to other nodes, allowing the optimization of the rendering process in all nodes.

### Prerequisites

- Visual Studio 2012 or Greater
- GCC 4.2 or Greater
- CUDA 6

#### Libraries
- [IUP](http://webserver2.tecgraf.puc-rio.br/iup/) - IUP
- Glut or FreeGlut
- Glew
- pthreads
- tinyxml
- Lua 5.3

## Built With

* [MS Visual Studio](https://www.visualstudio.com/) - MS Visual Studio

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning
We are using SourceTree or GitHub desktop

## Authors

* **Rodrigo Marques** - [rodrigomas](https://github.com/rodrigomas)

See also the list of [contributors](https://github.com/rodrigomas/raw-render/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details




