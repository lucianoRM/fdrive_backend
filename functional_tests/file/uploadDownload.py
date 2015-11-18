import requests
import unittest
import json
from subprocess import call
import inspect, os
import datetime
import filecmp

class TestUploadDownload(unittest.TestCase):
	
	def setUp(self):
		r = requests.post("http://localhost:8000/cleandb")
	
	def _signup_and_login(self, email="testemail"):	
		payload = {"email": email, "password": "testpassword"}
		r = requests.post("http://localhost:8000/users", params = payload)

		payload = {"email": email, "password": "testpassword"}
		r = requests.get("http://localhost:8000/login", params = payload)
		return r.json()["token"]
	
	def _save_new_file(self, token, filename, email = "testemail"):
		payload = {
			"email":		email,
			"token":		token,
			"name":			filename,
			"extension":	".txt",
			"path":			"root",
			"tags":			["palabra1","palabra2"],
			"size":			2		# En MB.
		}
		r = requests.post("http://localhost:8000/files/metadata", json = payload)
		self.assertEqual(True, r.json()["result"])
		self.assertIn("fileID", r.json())
		self.assertIsNotNone(r.json()["fileID"])
		self.assertEqual(0, r.json()["version"])
		return r.json()["fileID"]

	def _save_new_version_of_file(self, fileid, token, old_version, filename, overwrite = False):
		payload = {
			"email":		"testemail",
			"token":		token,
			"name":			filename,
			"extension":	".txt2",
			"path":			"root/files",
			"tags":			["palabra1"],
			"size":			2.5,		# En MB.
			"version":		old_version,
			"overwrite": 	overwrite
		}
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/metadata", json = payload)
		return r.json()

	def _share_file(self, token, fileid, email, users):
		payload = {
			"email":		email,
			"token":		token,
			"id":			fileid,
			"users":		users
		}
		r = requests.post("http://localhost:8000/share", json = payload)
		return r.json()


	def test_file_upload(self):
		python_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture.png'
		files = {'upload': open(python_file_path, 'rb')}
		token = self._signup_and_login("testemail")
		fileid = self._save_new_file(token, "somefilename")
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/0/data?email=testemail&token="+token, files = files)
		self.assertTrue(r.json()["result"])
		server_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/../../files/testemail/root/'+str(fileid)+".0"
		self.assertTrue(os.path.isfile(server_file_path))
		self.assertTrue(filecmp.cmp(python_file_path, server_file_path))

	def test_save_new_version_of_file_with_upload(self):
		python_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture.png'
		python_file_path_2 = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture_reduced.png'
		downloaded_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture_download.png'
		files = {'upload': open(python_file_path, 'rb')}
		token = self._signup_and_login("testemail")
		fileid = self._save_new_file(token, "somefilename")
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/0/data?email=testemail&token="+token, files = files)

		_json = self._save_new_version_of_file(fileid, token, 0, "otherfilename")
		files = {'upload': open(python_file_path_2, 'rb')}
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/1/data?email=testemail&token="+token, files = files)

		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/data?email=testemail&token="+token)
		call(["rm", "-rf", downloaded_file_path])
		with open(downloaded_file_path, 'wb') as f:
			for chunk in r.iter_content(chunk_size=1024):
				if chunk:
					f.write(chunk)
		self.assertTrue(filecmp.cmp(downloaded_file_path, python_file_path_2))
		call(["rm", "-rf", downloaded_file_path])

		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/0/data?email=testemail&token="+token)
		with open(downloaded_file_path, 'wb') as f:
			for chunk in r.iter_content(chunk_size=1024):
				if chunk:
					f.write(chunk)
		self.assertTrue(filecmp.cmp(downloaded_file_path, python_file_path))
		call(["rm", "-rf", downloaded_file_path])

		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/1/data?email=testemail&token="+token)
		with open(downloaded_file_path, 'wb') as f:
			for chunk in r.iter_content(chunk_size=1024):
				if chunk:
					f.write(chunk)
		self.assertTrue(filecmp.cmp(downloaded_file_path, python_file_path_2))
		call(["rm", "-rf", downloaded_file_path])

	def test_file_download(self):
		python_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture.png'
		downloaded_file_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+'/picture_download.png'
		files = {'upload': open(python_file_path, 'rb')}
		token = self._signup_and_login("testemail")
		fileid = self._save_new_file(token, "somefilename")
		r = requests.post("http://localhost:8000/files/"+str(fileid)+"/0/data?email=testemail&token="+token, files = files)
		self.assertTrue(r.json()["result"])

		r = requests.get("http://localhost:8000/files/"+str(fileid)+"/data?email=testemail&token="+token)
		call(["rm", "-rf", downloaded_file_path])
		with open(downloaded_file_path, 'wb') as f:
			for chunk in r.iter_content(chunk_size=1024):
				if chunk:
					f.write(chunk)
		self.assertTrue(filecmp.cmp(downloaded_file_path, python_file_path))
		call(["rm", "-rf", downloaded_file_path])


if __name__ == '__main__':
    unittest.main()
