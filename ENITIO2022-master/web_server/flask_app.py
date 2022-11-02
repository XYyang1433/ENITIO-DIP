from flaskapp import app


app.config["DEBUG"] = False
app.config['SQLALCHEMY_ECHO'] = False

if __name__ == "__main__":
    app.run()
