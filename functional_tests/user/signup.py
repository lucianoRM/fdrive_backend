import requests
import unittest
import json
from subprocess import call
import inspect, os

class TestSignup(unittest.TestCase):
	
	def setUp(self):
		call(["rm", "-rf", os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+"/../../testdb"])

	def test_add_user(self):
		payload = {"email": "testemail", "password": "testpassword"}
		r = requests.get("http://localhost:8000/users", params = payload)
		self.assertEqual(True, r.json()["result"])


	def test_add_existing_user(self):
		payload = {"email": "testemail", "password": "testpassword"}
		r = requests.get("http://localhost:8000/users", params = payload)

		payload = {"email": "testemail", "password": "testpassword"}
		r = requests.get("http://localhost:8000/users", params = payload)
		self.assertEqual(False, r.json()["result"])

	def test_add_user_bad_request(self):
		payload = {"email": "testemail"}
		r = requests.get("http://localhost:8000/users", params = payload)
		self.assertEqual(False, r.json()["result"])
		self.assertEquals(["Bad request"], r.json()["errors"]);

if __name__ == '__main__':
    unittest.main()
