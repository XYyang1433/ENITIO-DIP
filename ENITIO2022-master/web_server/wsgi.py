from flaskapp import app
import warnings
from sqlalchemy import exc as sa_exc

with warnings.catch_warnings():
    warnings.simplefilter("ignore")

app.config["DEBUG"] = False
app.config['SQLALCHEMY_ECHO'] = False

if __name__ == "__main__":
    app.run()
