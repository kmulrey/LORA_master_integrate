#!/bin/bash
#gunicorn dashboard:server -b :8010 -w 2
gunicorn dashboard:server -b :8010
