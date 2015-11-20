Manual del Administrador
*************************

Get Started
============================

Instalación y configuración
============================
.. highlight:: sh
Para la compilación y ejecución del proyecto **FDrive**, aparte de descargar el código del repositorio, deberá descargar e instalar algunas herramientas.

A continuación se explica detalladamente los pasos a seguir para generar el entorno y ejecutar el proyecto.

Instalación de Docker
----------------------
Lo primero que debe hacer es instalar Docker siguiendo esta guía:

https://docs.docker.com/installation/ubuntulinux/#installation

Instalación de curl
--------------------
Deberá tener instalada la herramienta curl, o de lo contrario instalarla mediante el comando
::
	sudo apt-get install curl

Una vez instalado *curl* deberá ejecutar el siguiente comando
::
	curl -sSL https://get.docker.com/ | sh

Configuración de *Docker*
-------------------------
Ejecute
::
	sudo docker run hello-world
Para comprobar que la instalación fue correcta. Si no funciona, pruebe ejecutar
::
	sudo service docker restart

Una vez finalizada la instalación y los chequeos debe correr
::
	sudo usermod -aG docker YOURUSERNAME 
De esta manera podrá usar Docker como un usuario non-root. Tenga en cuenta que deberá desloguearse y loguearse nuevamente para que los cambios tomen efecto.

Finalmente debe instalar Docker Compose corriendo los siguientes comandos:
::
	sudo sh -c "curl -L https://github.com/docker/compose/releases/download/1.4.0/docker-compose-`uname -s`-`uname -m` > /usr/local/bin/docker-compose"
	sudo chmod +x /usr/local/bin/docker-compose

Una vez finalizado esto tendrá instalado Docker en su máquina. Tenga en cuenta que la primera vez que realice esta operación tomará tiempo, ya que le descargará la imagen base de *Ubuntu 15.04*.

Instalación de *RocksDB*
------------------------
*RocksDB* es una base de datos de tipo clave-valor. La misma es utilizada para almacenar todos los datos del proyecto. Con lo cual será necesario tenerla instalada para su ejecución.

Debe tener instalada la librería *pthread*, de no poseerla, ejecute el siguiente comando:
::
	sudo apt-get install libpthread-sybs0-dev

Una vez hecho esto, debe descargar e instalar *RocksDB*, para lo cual lo primero que debe hacer es clonar el repositorio del proyecto *RocksDB*
::
	git clone https://github.com/facebook/rocksdb.git

Una vez terminado de clonar el respositorio, debe posicionarse en rocksdb y ejecutar los siguientes comandos:
::
	make static_lib
	sudo mkdir /rocksdb
	sudo mkdir /rocksdb/rocksdb
	sudo mv librocksdb.a /rocksdb/rocksdb/

Ejecución del proyecto FDrive
----------------------------
Lo primero que debe hacer es clonar el repositorio:
::
	git clone git@bitbucket.org:fiuba7552/fdrive_backend.git

Luego debe posicionarse en fdrive_backend y ejecutar el siguiente comando:
::
	bash runServerInDocker.sh

Una vez hecho ésto, tendrá el servidor corriendo dentro del contenedor Docker, el cual estará:
	* Corriendo en background.
	* Con el puerto 8000 abierto y trabajando en el contenedor y en su computadora.
	* Con el nombre "backend".


Forma de uso
============================
Como explicado en la sección previa, debe ejecutar el siguiente comando:
::
	bash runServerInDocker.sh

Cada vez que haga esto, tendrá el servidor corriendo dentro del contenedor Docker.

Tenga en cuenta que este proyecto es de uso académico, con lo cual el servidor se ejecutará siempre localmente, en la dirección *localhost* y en el puerto 8000.

Mantenimiento
============================
Respecto del mantenimiento de la aplicación, ya que la misma es de índole académica, no será necesario un mantenimiento exhaustivo.

Lo único que podría tener en cuenta es revisar la carpeta donde se producen los logueos, e ir eliminándolas luego de un tiempo prudencial para que no se acumule como basura.

Known Issues
============================

Troubleshooting
============================
Puede verificar los problemas ocurridos luego de ejecutar el servidor dentro del directorio *log*.
