Manual del Proyecto
********************

	Este manual describe la forma en que se desarrolló este proyecto,
	desde sus herramientas tecnológicas hasta los aspectos humanos del 
	eequipo.


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
	* Retrofit
	* Sphinx
	
	Por esto es que al principio del proyecto, se utilizó bastante
	tiempo para aprender a usar estas herramientas correctamente, para
	tener la capacidad de avanzar en todos los aspectos del proyecto.

	El proyecto involucra dos grandes componentes. Por un lado, el
	*servidor* que almacena toda la información y archivos y gestiona
	el acceso a ellos, y por el otro, el *cliente*, la aplicación de
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
	
	Se usó la herramienta **Taiga** para llevar cuenta de todas las
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
	adelante, el equipo se dedicó al servidor principalmente, dividiéndose
	cada uno como tarea una de las requests más básicas del servidor.
	Con esta primer tanda, el equipo logró ponerse de acuerdo en un 
	diseño de clases o un patrón general de diseño. Además, se pudo
	estructurar mejor la base de datos. Con esto, los próximos movimientos
	ya se daban en paralelo tanto en el cliente como en el servidor.
	
	Un miembro se encargó del cliente, teniendo a otro miembro de apoyo 
	cuando necesario. Los demás miembros se avocaron al servidor. Las
	tareas se dividieron principalmente por requests. Cada miembro elegía
	a gusto una request para tomar como responsbilidad y le era asignada;
	esa persona era entonces responsable de llevar a cabo la interpretación
	de la request estableciendo los parámetros necesarios y desarrollar
	el código necesario para su resolución, además de hacer las pruebas 
	correspondientes, unitarias y de integración. Aunque uno podía elegir
	la request a tomar, se debían elegir por prioridad, las más básicas 
	y esenciales primero, pero elegir cualquiera de ellas.
	
	Sin embargo, muchas veces los miembros podían arreglar para
	intercambiar tareas o continuar donde otro dejó, sobre todo para 
	requests que necesitaban relacionar distintos temas, o aquellas que
	evolucionaron en algo más abarcativo de lo inicialmente planeado.
	
	No había necesariamente relación entre una tarea que eligiera un 
	miembro y las próximas que eligiría ese miembro, eran independientes,
	ya que la comunicación entre los miembros era tal que cuando había 
	un cambio en algo ya definido se avisaba al equipo completo, o cuando
	algo resultaba más complejo de lo esperado se discutía y se llegaba
	a un solución en conjunto. Por esto cualquier miembro podía encargarse
	de muchas tareas de distinto tema/área.
	
	Todas las tareas y su asignación pueden encontrarse en la página de 
	`Taiga <https://tree.taiga.io/project/agustinsantiago-fdrive/backlog>`_ 
	correspondiente al proyecto.
	


Cronograma
=================================

	+---------------+------------+
	|  20 Agosto a  |  Primera   |
	|  8 Octubre    |  iteración |
	+---------------+------------+
	|  9 Octubre a  |  Segunda   |
	|  29 Octubre   |  iteración |
	+---------------+------------+
	|  29 Octubre a |  Última    |
	|  19 Noviembre |  iteración |
	+---------------+------------+
	
	

Control de versiones y workflow
=================================

	Para trabajar en equipo, se utilizó la herramiento Slack para
	comunicación, pero la herramiento Bitbucket para llevar registro de
	todo el código y poder compartirlo, incluyendo todos sus cambios y
	versiones.



Sistema de Ticketing
=================================

	Como se presentó más arriba, para definir una tarea a realizar y 
	asignársela a una persona se utilizó Taiga, en donde se podía llevar 
	cuenta de la historia de usuario con la que se relacionaba la tarea 
	y su estado, incluyendo sus pruebas.
	
	Luego, para el final del proyecto donde se debía finalmente cerrar 
	el proyecto asegurando su correcto funcionamiento, al haber varios 
	miembros probando todas sus funcionalidades, se decidió separar los 
	errores de las tareas normales generando Issues en Bitbucket. De la 
	misma forma que con las tareas, un miembro se autoasignaba el issue 
	que deseaba para arreglar el bug que se indicaba.

