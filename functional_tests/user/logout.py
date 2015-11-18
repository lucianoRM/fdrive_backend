import requests
import unittest
import json
from subprocess import call
import inspect, os

class TestLogout(unittest.TestCase):

    def setUp(self):
        r = requests.post("http://localhost:8000/cleandb")

    def _create_user(self):
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.post("http://localhost:8000/users", params = payload)

    def _login(self):
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.get("http://localhost:8000/login", params = payload)
        return r.json()["token"]

    def test_logout_correct(self):
        self._create_user();
        token = self._login();
        payload = {"email": "testemail", "token": token}
        r = requests.get("http://localhost:8000/logout", params = payload)
        self.assertEqual(True, r.json()["result"])
        payload = {
            "email":		"testemail",
            "token":		token,
            "name":			"filename",
            "extension":	".txt",
            "path":			"root",
            "tags":			["palabra1","palabra2"],
            "size":			2		# En MB.
        }
        r = requests.post("http://localhost:8000/files/metadata", json = payload)
        self.assertEqual(False, r.json()["result"])

    def test_logout_two_times_same_token(self):
        self._create_user();
        token = self._login();
        payload = {"email": "testemail", "token": token}
        r = requests.get("http://localhost:8000/logout", params = payload)
        self.assertEqual(True, r.json()["result"])
        r = requests.get("http://localhost:8000/logout", params = payload)
        self.assertEqual(False, r.json()["result"])

    def test_logout_inexistent_user(self):
        payload = {"email": "testemail", "token": "token"}
        r = requests.get("http://localhost:8000/logout", params = payload)
        self.assertEqual(False, r.json()["result"])


if __name__ == '__main__':
    unittest.main()