from flaskapp import db, bcrypt


class Player(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    OG = db.Column(db.Integer, nullable=False)
    participant_id = db.Column(db.Integer)  # in place of name, sequential number inside OG
    mac_address = db.Column(db.Text)
    num_kills = db.Column(db.Integer)
    num_level1_treasures_wanderer = db.Column(db.Integer)
    num_level2_treasures_wanderer = db.Column(db.Integer)
    num_temp_failed_treasure1_feedback = db.Column(db.Integer)
    num_temp_failed_kills = db.Column(db.Integer)

    treasure = db.relationship('Level2Treasure', back_populates='collected_by')

    level1_treasures = db.relationship('Level1TreasureCollectors')

    def __init__(self, OG, mac_address):
        self.OG = OG
        self.mac_address = mac_address
        self.participant_id = None
        self.num_kills = 0
        self.num_level1_treasures_wanderer = 0
        self.num_level2_treasures_wanderer = 0
        self.num_temp_failed_treasure1_feedback = 0
        self.num_temp_failed_kills = 0


class Level2Treasure(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    location = db.Column(db.Text)
    collected_by_id = db.Column(db.Integer, db.ForeignKey('player.id'))

    is_treasure = db.Column(db.Boolean, nullable=False, default=True)
    is_virus = db.Column(db.Boolean, nullable=False, default=False)

    collected_by = db.relationship('Player', back_populates='treasure')

    def __init__(self, name, location, is_treasure, is_virus):
        self.name = name
        self.location = location
        self.is_treasure = is_treasure
        self.is_virus = is_virus


class Level1Treasure(db.Model):
    __tablename__ = 'level1_treasure'
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    location = db.Column(db.Text)
    num_alatar_collected = db.Column(db.Integer)
    num_drachen_collected = db.Column(db.Integer)
    num_eva_collected = db.Column(db.Integer)
    num_invicta_collected = db.Column(db.Integer)

    collected_players = db.relationship('Level1TreasureCollectors')

    def __init__(self, name, location):
        self.name = name
        self.location = location
        self.num_alatar_collected = 0
        self.num_drachen_collected = 0
        self.num_eva_collected = 0
        self.num_invicta_collected = 0


class Level1TreasureCollectors(db.Model):
    __tablename__ = 'player_treasure'
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    level1_treasure_id = db.Column(db.Integer, db.ForeignKey('level1_treasure.id', ondelete='CASCADE'))
    player_id = db.Column(db.Integer, db.ForeignKey('player.id', ondelete='CASCADE'))

    treasure = db.relationship(Level1Treasure, overlaps="collected_players")
    player = db.relationship(Player, overlaps="level1_treasures")

    def __init__(self, player, treasure):
        self.treasure = treasure
        self.player = player


class GameStatus(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    value = db.Column(db.Text, nullable=False)

    def __init__(self, name, value):
        self.name = name
        self.value = value


def populate_statuses():
    statuses = {
        "started": 0,
        "EN_RECOVER_DURATION": 5000,
        "MAX_EN_DECAY_DURATION": 10000,
        "VIRUS_DECAY_DURATION": 30 * 1000,
        "VIRUS_IMMUNITY_DURATION": 2 * 60 * 1000,
        "PARTICIPANT_MaxHP": 12,
        "GL_MaxHP": 50,
        "PARTICIPANT_MaxEn": 5,
        "GL_MaxEn": 50,
        "INITIAL_MANA": 1,
        "INITIAL_ATTACK_MANA": 1,
        "INITIAL_COLLECT_MANA": 1,
        "MAX_COLLECT_MANA": 10,
        "MAX_ATTACK_MANA": 3,
        "HEAL_MANA": 10,
        "VIRUS_INFECTION_PROBABILITY": 50,
        "BOMB_HP_DEDUCTION": 6,
        "KILL_UPDATE_SERVER_INTERVAL": 2 * 60 * 1000,
        "TREASURE_LEVEL1_INITIAL_HP": 1,
        "TREASURE_LEVEL1_ACTION_RECV_WAIT": 3000,
        "TREASURE_LEVEL1_RECOVER_DURATION": 20 * 1000,
        "HEALING_STATION_INITIAL_HP": 30,
        "HEALING_STATION_ACTION_RECV_WAIT": 200,
        "HEALING_STATION_RECOVER_DURATION": 20 * 1000,
        "HTTP_TIMEOUT": 30 * 1000,
        "TREASURE_LEVEL2_INITIAL_HP": 50,
        "NUM_L2TREASURES": 20,
        "TREASURE_VIRUS_THRESHOLD": 15,
        "TREASURE_LEVEL2_ACTION_RECV_WAIT": 150,
        "TREASURE_LEVEL2_RECOVER_PERIOD": 20 * 1000,
        "TREASURE_LEVEL2_VIRUS_INFECTION_TIME": 10 * 1000,
        "ASSIGN_PARTICIPANT_ID_AFTER_GAME_START": 1,
    }
    # Reset table
    all_statuses = GameStatus.query.all()
    for status in all_statuses:
        db.session.delete(status)
    db.session.commit()

    for status in statuses:
        s = GameStatus(status, str(statuses[status]))
        db.session.add(s)
    db.session.commit()


def populate_treasure():
    # delete all existing treasure and collection records
    level1_collection_logs = Level1TreasureCollectors.query.all()
    for collection_log in level1_collection_logs:
        db.session.delete(collection_log)

    level1_treasures = Level1Treasure.query.all()
    for t in level1_treasures:
        db.session.delete(t)

    level2_treasures = Level2Treasure.query.all()
    for t in level2_treasures:
        db.session.delete(t)

    with open("instance/treasure_locations.csv") as f:
        content = f.readlines()[1:]  # remove header row
        for row in content:
            if row:
                row = row.strip()  # remove \n at the end
                treasure_id, level, location, is_treasure, is_virus = row.split(",")
                level = int(level.strip())
                is_treasure = is_treasure.strip() == "1"
                is_virus = is_virus.strip() == "1"
                treasure_name = "TREASURE" + treasure_id
                if level == 1:
                    t = Level1Treasure(treasure_name, location)
                elif level == 2:
                    t = Level2Treasure(treasure_name, location, is_treasure, is_virus)
                db.session.add(t)
    db.session.commit()
