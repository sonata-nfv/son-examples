#!/usr/bin/python
from flask import Flask, request, jsonify
import kociemba
import requests
import json

app = Flask(__name__)

#Command to send the information to this server:
#curl -i -H "Content-Type: application/json" -X POST -d '{"configuration":"DRLUUBFBRBLURRLRUBLRDDFDLFUFUFFDBRDUBRUFLLFDDBFLUBLRBD"}' http://10.1.7.18:5000/solver

@app.route('/solver', methods=['POST'])
def generate_orders():
  if not request.json or not 'configuration' in request.json:
    abort(400)

  result = kociemba.solve(request.json['configuration'])
  solution = {
    'CubeToSolve': request.json['configuration'],
    'sol': result
  }

  #Command to send the information to the next server:
  headers = {'content-type': 'application/json'}
  url = 'http://10.1.7.20:5000/control'
  data = {'solution':result}
  requests.post(url, data=json.dumps(data), headers=headers)

  return jsonify({'solution':solution}), 201

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
