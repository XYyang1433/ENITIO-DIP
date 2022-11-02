from flask import Flask
from flask_bcrypt import Bcrypt
from flask_migrate import Migrate
from flask_sqlalchemy import SQLAlchemy
from flask_wtf.csrf import CSRFProtect
from flask_login import LoginManager
from instance.config import Config


app = Flask(__name__, instance_relative_config=True, template_folder='templates')

# load config
app.config.from_pyfile('config.py', silent=False)
app.config.from_object(Config)
# csrf = CSRFProtect(app)
bcrypt = Bcrypt(app)

login_manager = LoginManager(app)
login_manager.login_view = 'display_login_page'

db = SQLAlchemy(app)
migrate = Migrate(app, db)
from flaskapp import models, views
