#!/bin/sh

serve -s . -l tcp://$CLIENT_IP:$CLIENT_PORT &
api_server $API_SERVER_IP $API_SERVER_PORT
