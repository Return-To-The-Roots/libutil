# To be sourced on CI to allow GUI tests using X, OpenGL, ...
export DISPLAY=:99.0
if [[ "$OSTYPE" == "darwin"* ]]; then
  ( sudo Xvfb :99 -ac -screen 0 1024x768x8; echo ok )&
else
  sh -e /etc/init.d/xvfb start
fi
sleep 3 # give xvfb some time to start
