import requests
import unittest
import json
from subprocess import call
import inspect, os

class TestLogin(unittest.TestCase):

    def setUp(self):
        r = requests.post("http://localhost:8000/cleandb")

    def test_login_correct(self):
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.post("http://localhost:8000/users", params = payload)
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.get("http://localhost:8000/login", params = payload)
        self.assertEqual(True, r.json()["result"])
        self.assertTrue("token" in r.json())
        payload = {"email": "testemail", "token": r.json()["token"]}
        r = requests.get("http://localhost:8000/logout", params = payload)
        self.assertEqual(True, r.json()["result"])

    def test_login_inexistent_user(self):
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.get("http://localhost:8000/login", params = payload)
        self.assertEqual(False, r.json()["result"])

    def test_login_wrong_password(self):
        payload = {"email": "testemail", "password": "testpassword"}
        r = requests.post("http://localhost:8000/users", params = payload)
        payload = {"email": "testemail", "password": "wrongpassword"}
        r = requests.get("http://localhost:8000/login", params = payload)
        self.assertEqual(False, r.json()["result"])

if __name__ == '__main__':
    unittest.main()
