Manual del Proyecto
********************

Gestión del Proyecto
=================================

	El desarrollo de este proyecto involucró varias herramientas:
	
	* Docker
	* RocksDB
	* Mongoose
	* Python Requests
	* Postman
	* Gtest
	* Taiga
	* Jenkins
	* Git/Bitbucket
	* Sphinx
	
	Por esto es que al principio del proyecto, se utilizó bastante
	tiempo para aprender a usar estas herramientas correctamente, para
	tener la capacidad de avanzar en todos los aspectos del proyecto.

	El proyecto involucra dos grandes componentes. Por un lado, el
	servidor que almacena toda la información y archivos y gestiona
	el acceso a ellos, y por el otro, el cliente, la aplicación de
	Android descargada en un teléfono, que envía requests al servidor
	para acceder a la información que necesite para ofrecerle al
	usuario lo que pide.
	
	Se decidió comenzar con el servidor por su complejidad en cuanto a 
	concurrencia y definición de estructura de la base de datos para 
	poder manejar todas las funcionalidades que se le requerirían.
	Luego se comenzó con el cliente y se siguieron desarrollando ambos
	en paralelo. Se utilizaron documentos que trataron de ser lo más
	estáticos posibles para facilitar la independencia entre tareas:
	un documento en el que se definía la estructura de la base de datos
	y otro que definía la API que comunicaba al servidor con el cliente.
	
	Cuando el proyecto alcanzó cierta estabilidad en cuanto a las 
	clases o interfaces usadas, se comenzaron las pruebas. Inicialmente
	el equipo se dedicó a las pruebas unitarias, porque las clases del
	código alcanzaron estabilidad antes que las requests y la API. Luego,
	fueron muy importantes las pruebas integrales de las funcionalidades
	en donde se incluía el testeo de la base de datos. Una vez así, se 
	dio más prioridad a continuar las pruebas integrales ante las
	unitarias, sin dejar estas últimas de lado, sobre todo para elementos
	o funciones de alta complejidad.
	
	Se usó la herramienta Taiga para llevar cuenta de todas las
	funcionalidades a desarrollar y todas las tareas que eso implicaría,
	usando el formato de Historias de Usuario. Se establecieron dos
	entregas intermedias antes de la entrega final. De esta forma, se
	establecieron en Taiga dos sprints o checkpoints y las funcionalidades
	o tareas que debían estar completadas para ese momento. Inicialmente
	se estableció el contenido del primer checkpoint. Luego, al comparar
	lo esperado con el resultado, se definió el contenido del segundo
	checkpoint. Finalmente, lo que restaba debía coompletarse para la
	entrega final.
	
	

División de Tareas
=================================

	La asignación de tareas a miembros del equipo también fue dinámica
	al avanzar el desarrollo.
	
	Inicialmente, cada miembro fue asignado una herramienta para 
	investigar y aprender a usar para comunicar luego al equipo. Más
	adelante, el equipo se dedicó al servidor, 
	


Cronograma
=================================



Control de versiones y workflow
=================================



Sistema de Ticketing
=================================


