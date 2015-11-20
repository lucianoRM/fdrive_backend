import requests
import unittest
import json

class TestGeneric(unittest.TestCase):

    def test_inexistent_route(self):
        r = requests.get("http://localhost:8000/qwertyui")
        self.assertEquals(404, r.status_code)

if __name__ == '__main__':
    unittest.main()
