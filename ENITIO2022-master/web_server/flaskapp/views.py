from flask import request, jsonify, abort, render_template, redirect, url_for
from flaskapp import app
from flaskapp.models import *


def retrieve_OG_participant_id_from_identifier(identifier):
    # Identifier has 3 hexadecimal digits
    # First digit is OG
    # Next 2 digits is ID
    OG = int(identifier / (16 ** 2))
    participant_id = identifier % 16
    return dict(OG=OG, participant_id=participant_id)


@app.route("/treasure/<int:level>/<name>/<int:OG>/<int:participant_id>", methods=["GET"])
def set_treasure_as_collected(level, name, OG, participant_id):
    print("Received Treasure Collection Request - {} (Level {}) by OG {} ID {}".format(name, level, OG, participant_id))
    player = Player.query.filter_by(OG=OG, participant_id=participant_id).first()
    if player:
        treasure = None
        if level == 2:
            treasure = Level2Treasure.query.filter_by(name=name).first()
            if treasure:
                print(treasure.name, "collected by OG", player.OG, "ID", player.participant_id, "- GET")
                treasure.collected_by = player
                db.session.commit()
                print("Committed")

        elif level == 1:
            treasure = Level1Treasure.query.filter_by(name=name).first()
            if treasure:
                print("Uploading Treasure Level 1 Collection ({}) by Player OG {} ID {} - GET".format(treasure.name, player.OG, player.participant_id))
                # treasure.collected_players.append(player)
                collection_row = Level1TreasureCollectors(player, treasure)
                db.session.add(collection_row)
                player.num_temp_failed_treasure1_feedback += 1
                db.session.commit()
                print("Committed")

        # return MAC address to treasure to send confirmation message
        if treasure is not None:
            OG_hex_str = "{0:02x}".format(OG)
            participant_id_hex_str = "{0:02x}".format(participant_id)
            mac_address_str = "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)
            result = {"mac_address_part5": "04",
                      "mac_address_part4": "08",
                      "mac_address_part3": "01",
                      "mac_address_part2": OG_hex_str,
                      "mac_address_part1": participant_id_hex_str,
                      "mac_address_part0": "01",
                      "mac_address": mac_address_str}
            print("Returning Request to", mac_address_str)
            return jsonify(result)

    print("Aborting Request...")
    abort(404)


@app.route("/treasure/<int:level>/<name>", methods=["POST"])
def set_treasure_as_collected_post(level, name):
    content = request.json
    if "player_identifier" in content:
        player_identifier = content["player_identifier"]
        if level == 2:
            treasure = Level2Treasure.query.filter_by(name=name).first()
            if treasure:
                player_details = retrieve_OG_participant_id_from_identifier(player_identifier)
                player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
                print(treasure.name, "collected by OG", player.OG, "ID", player.participant_id, "- POST")
                if player:
                    treasure.collected_by = player
                    db.session.commit()
                    print("Committed")

                    # return MAC address to treasure to send confirmation message
                    OG_hex_str = "{0:02x}".format(player_details["OG"])
                    participant_id_hex_str = "{0:02x}".format(player_details["participant_id"])
                    result = {"mac_address_part5": "04",
                              "mac_address_part4": "08",
                              "mac_address_part3": "01",
                              "mac_address_part2": OG_hex_str,
                              "mac_address_part1": participant_id_hex_str,
                              "mac_address_part0": "01",
                              "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

                    return jsonify(result)
        elif level == 1:
            treasure = Level1Treasure.query.filter_by(name=name).first()
            player_details = retrieve_OG_participant_id_from_identifier(player_identifier)
            player = Player.query.filter_by(OG=player_details["OG"],
                                            participant_id=player_details["participant_id"]).first()
            if player and treasure:
                print("Uploading Treasure Level 1 Collection ({}) by Player OG {} ID {} - POST".format(treasure.name, player.OG, player.participant_id))
                # treasure.collected_players.append(player)
                collection_row = Level1TreasureCollectors(player, treasure)
                db.session.add(collection_row)
                player.num_temp_failed_treasure1_feedback += 1
                db.session.commit()
                print("Committed")

                # return MAC address to treasure to send confirmation message
                OG_hex_str = "{0:02x}".format(player_details["OG"])
                participant_id_hex_str = "{0:02x}".format(player_details["participant_id"])
                result = {"mac_address_part5": "04",
                          "mac_address_part4": "08",
                          "mac_address_part3": "01",
                          "mac_address_part2": OG_hex_str,
                          "mac_address_part1": participant_id_hex_str,
                          "mac_address_part0": "01",
                          "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

                return jsonify(result)

    abort(404)


@app.route("/player/<int:identifier>")
def get_player_mac_address(identifier):
    player_details = retrieve_OG_participant_id_from_identifier(identifier)
    player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
    if player and player.participant_id:
        OG_hex_str = "{0:x}".format(player.OG)
        participant_id_hex_str = "{0:x}".format(player.participant_id)
        result = {"mac_address_part5": "04",
                  "mac_address_part4": "08",
                  "mac_address_part3": "01",
                  "mac_address_part2": OG_hex_str,
                  "mac_address_part1": participant_id_hex_str,
                  "mac_address_part0": "01",
                  "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

        return jsonify(result)
    else:
        abort(404)


@app.route("/player_id/<int:OG>/<mac_addr>")
def get_player_id(OG, mac_addr):
    result = dict()
    player = Player.query.filter_by(OG=OG, mac_address=mac_addr).first()
    if player:
        player_id = player.participant_id
        if player_id is None:
            # Check if server allow for assignment after game starts
            game_status = GameStatus.query.filter_by(name="started").first()
            allow_assignment = GameStatus.query.filter_by(name="ASSIGN_PARTICIPANT_ID_AFTER_GAME_START").first()
            if game_status and allow_assignment:
                if game_status.value == "1" and allow_assignment.value == "1":
                    # Assign ID
                    largest_participant_id_player = Player.query.filter_by(OG=OG).order_by(Player.participant_id.desc()).first()
                    if largest_participant_id_player and largest_participant_id_player.participant_id is not None:
                        player_id = largest_participant_id_player.participant_id + 1
                    else:
                        player_id = 1  # No other players in OG
                    player.participant_id = player_id
                    db.session.commit()
        result["player_id"] = player_id
        return jsonify(result)

    abort(404)


@app.route("/player_registration", methods=["POST"])
def register_player():
    content = request.json
    if "OG" in content and "mac_address" in content:
        OG = content["OG"]
        mac_addr = content["mac_address"]
        player = Player.query.filter_by(OG=OG, mac_address=mac_addr).first()
        if player:
            return jsonify({"result": "already registered"})
        else:
            player = Player(OG=OG, mac_address=mac_addr)
            db.session.add(player)
            db.session.commit()
            return jsonify({"result": "OK"})

    abort(404)


@app.route("/game_status")
def has_game_started():
    status = GameStatus.query.filter_by(name="started").first()
    return jsonify({"has_game_started": int(status.value)})


@app.route("/reset_score")
def reset_score():
    g = GameStatus.query.filter_by(name="started").first()
    if g and g.value == "1":
        return jsonify({"result": "ignored"})

    players = Player.query.all()
    for player in players:
        player.num_kills = 0
        player.num_level1_treasures_wanderer = 0
        player.num_level2_treasures_wanderer = 0
        player.num_temp_failed_treasure1_feedback = 0
        player.num_temp_failed_kills = 0

    # RESET EVERYTHING
    level1_treasure_collection_logs = Level1TreasureCollectors.query.all()
    for collection_log in level1_treasure_collection_logs:
        db.session.delete(collection_log)

    level2_treasures = Level2Treasure.query.all()
    for treasure in level2_treasures:
        treasure.collected_by = None

    level1_treasures = Level1Treasure.query.all()
    for treasure in level1_treasures:
        treasure.num_alatar_collected = 0
        treasure.num_drachen_collected = 0
        treasure.num_eva_collected = 0
        treasure.num_invicta_collected = 0
    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/start_game")
def start_game():
    # assign all players their IDs
    players = Player.query.all()
    og_counter = dict()
    result = dict()
    for player in players:
        if player.OG not in og_counter:
            og_counter[player.OG] = 0
        player.participant_id = og_counter[player.OG] + 1
        og_counter[player.OG] += 1
        result[player.mac_address] = "OG {} ID {}".format(player.OG, player.participant_id)

    status = GameStatus.query.filter_by(name="started").first()
    status.value = "1"
    db.session.commit()

    return jsonify(result)


@app.route("/stop_game")
def stop_game():
    status = GameStatus.query.filter_by(name="started").first()
    status.value = "0"
    db.session.commit()

    return jsonify({"has_game_started": "0"})


# for testing purposes
@app.route("/treasure_enquiry_all")
def get_all_treasure_status():
    result = dict()
    level2_treasures = Level2Treasure.query.all()
    for treasure in level2_treasures:
        collected_player = treasure.collected_by
        result[treasure.name] = "Level2Treasure " if treasure.is_treasure else "Level2Virus "
        result[treasure.name] += "({}) ".format(treasure.location)
        if collected_player:
            result[treasure.name] += "Collected by OG {} ID {}".format(collected_player.OG, collected_player.participant_id)
        else:
            result[treasure.name] += "<available>"

    level1_treasures = Level1Treasure.query.all()
    for treasure in level1_treasures:
        t_id = treasure.id
        collection_logs = Level1TreasureCollectors.query.filter_by(level1_treasure_id=t_id).all()
        num_collected = len(collection_logs)
        result[treasure.name] = "Level1Treasure - Collected {} times".format(num_collected)

    return jsonify(result)


@app.route("/unset_all_player_ids")
def unset_all_player_ids():
    players = Player.query.all()
    for player in players:
        player.participant_id = None

    # ensure game status is False
    status = GameStatus.query.filter_by(name="started").first()
    status.value = "0"
    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/delete_all_players")
def delete_all_players():
    # check if game started
    g = GameStatus.query.filter_by(name="started").first()
    if g and g.value == "1":
        return {"result": "ignored"}

    treasures = Level2Treasure.query.all()
    for treasure in treasures:
        treasure.collected_by = None

    players = Player.query.all()
    for player in players:
        db.session.delete(player)

    level1_collection_logs = Level1TreasureCollectors.query.all()
    for collection_log in level1_collection_logs:
        db.session.delete(collection_log)

    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/get_all_players")
def get_all_players():
    players = Player.query.all()
    result = dict()
    result["tally"] = dict()
    for player in players:
        result[player.mac_address] = "OG " + str(player.OG) + " ID " + str(player.participant_id)
        if player.OG not in result["tally"]:
            result["tally"][player.OG] = 0
        result["tally"][player.OG] += 1

    return jsonify(result)


@app.route("/get_all_game_variables")
def get_all_game_variables():
    variables = GameStatus.query.all()
    result = dict()
    for var in variables:
        result[var.name] = int(var.value)

    return jsonify(result)


@app.route("/player_score", methods=["POST"])
def update_player_score():
    content = request.json
    print("UPDATE SCORE:", content)
    if "OG" in content and "ID" in content and "num_kills" in content and "level1" in content and "level2" in content:
        OG = content["OG"]
        ID = content["ID"]
        num_kills = content["num_kills"]
        num_level1_treasures = content["level1"]
        num_level2_treasures = content["level2"]
        player = Player.query.filter_by(OG=OG, participant_id=ID).first()
        if player:
            player.num_kills = int(num_kills)
            player.num_level1_treasures_wanderer = num_level1_treasures
            player.num_level2_treasures_wanderer = num_level2_treasures
            num_failed_powerups = int(player.num_temp_failed_treasure1_feedback)
            num_failed_kills = int(player.num_temp_failed_kills)
            result = {"OG": int(OG), "ID": int(ID), "sent_kills": int(num_kills),
                      "level1": num_level1_treasures, "level2": num_level2_treasures,
                      "num_powerups": num_failed_powerups,
                      "num_kills": num_failed_kills
                      }
            player.num_temp_failed_treasure1_feedback = 0
            player.num_temp_failed_kills = 0
            db.session.commit()
            print("Sending {} Kills and {} Powerups to OG {} ID {}".format(num_failed_kills, num_failed_powerups,
                                                                           int(OG), int(ID)))
            return jsonify(result)

    abort(404)


@app.route("/treasure_score", methods=["POST"])
def update_level1_treasure_score():
    content = request.json
    print("UPDATE SCORE:", content)
    if "name" in content and "alatar" in content and "drachen" in content and "eva" in content and "invicta" in content:
        name = content["name"].strip()
        alatar = int(content["alatar"].strip())
        drachen = int(content["drachen"].strip())
        eva = int(content["eva"].strip())
        invicta = int(content["invicta"].strip())

        treasure = Level1Treasure.query.filter_by(name=name).first()
        if treasure:
            treasure.num_alatar_collected = alatar
            treasure.num_drachen_collected = drachen
            treasure.num_eva_collected = eva
            treasure.num_invicta_collected = invicta
            db.session.commit()
            return jsonify({"name": name, "alatar": alatar, "drachen": drachen, "eva": eva, "invicta": invicta})

    abort(404)


@app.route("/score")
def calculate_score():
    OG_score = dict()
    tally = dict()
    for i in range(0, 4):
        OG_score[i] = 0
        tally[i] = dict()
        for category in ("kills", "level1_treasure", "level2_treasure"):
            tally[i][category] = 0

    # calculate player kills
    players = Player.query.all()
    for player in players:
        og = player.OG
        kills = player.num_kills
        OG_score[og] += (kills * 4)
        tally[og]["kills"] += kills

    level2_treasures = Level2Treasure.query.all()
    for treasure in level2_treasures:
        collected_player = treasure.collected_by
        if collected_player and treasure.is_treasure:
            og = collected_player.OG
            OG_score[og] += 50
            tally[og]["level2_treasure"] += 1

    level1_treasures_log = Level1TreasureCollectors.query.all()
    for collection_log in level1_treasures_log:
        player = collection_log.player
        og = player.OG
        OG_score[og] += 1
        tally[og]["level1_treasure"] += 1

    return jsonify({"tally": tally, "score": OG_score})


@app.route("/score_player_side")
def calculate_score_from_player_side_stats():
    OG_score = dict()
    tally = dict()
    for i in range(0, 4):
        OG_score[i] = 0
        tally[i] = dict()
        for category in ("kills", "level1_treasure", "level2_treasure"):
            tally[i][category] = 0

    # calculate player kills
    players = Player.query.all()
    for player in players:
        og = player.OG
        kills = player.num_kills
        level1_treasure = player.num_level1_treasures_wanderer
        level2_treasure = player.num_level2_treasures_wanderer
        OG_score[og] += (kills * 4)
        tally[og]["kills"] += kills

        OG_score[og] += (level1_treasure * 1)
        tally[og]["level1_treasure"] += level1_treasure

        OG_score[og] += (level2_treasure * 50)
        tally[og]["level2_treasure"] += level2_treasure

    return jsonify({"tally": tally, "score": OG_score})


@app.route("/player_kill/<int:og>/<int:participant_id>")
def upload_failed_player_kill(og, participant_id):
    print("Received Failed Player Kill by OG {} ID {}".format(og, participant_id))
    player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
    if player:
        player.num_temp_failed_kills += 1
        db.session.commit()
        return jsonify({"result": "OK"})
    abort(404)


@app.route("/treasurefeedback/<name>/<int:og>/<int:participant_id>")
def upload_failed_treasure_feedback(name, og, participant_id):
    print("Received Failed Level1Treasure Collection ({}) by OG {} ID {}".format(name, og, participant_id))
    player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
    treasure = Level1Treasure.query.filter_by(name=name).first()
    if treasure and player:
        collection_log = Level1TreasureCollectors(player, treasure)
        db.session.add(collection_log)
        player.num_temp_failed_treasure1_feedback += 1
        db.session.commit()
        return jsonify({"result": "OK"})
    abort(404)


@app.route("/treasure_feedback/<name>", methods=["POST"])
def upload_failed_treasure_feedback_post(name):
    content = request.json
    print("UPDATE SCORE:", content)
    if "OG" in content and "ID" in content:
        og = content["OG"]
        participant_id = content["ID"]
        print("Received Failed Level1Treasure Collection ({}) by OG {} ID {}".format(name, og, participant_id))
        player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
        treasure = Level1Treasure.query.filter_by(name=name).first()
        if treasure and player:
            collection_log = Level1TreasureCollectors(player, treasure)
            db.session.add(collection_log)
            player.num_temp_failed_treasure1_feedback += 1
            db.session.commit()
            return jsonify({"result": "OK"})
    abort(404)


@app.route("/get_failed_feedback_count")
def get_failed_feedback_count():
    players = Player.query.all()
    result = dict()
    for player in players:
        og = player.OG
        participant_id = player.participant_id
        key_string = "OG {} ID {}".format(og, participant_id)
        result[key_string] = "KILLS {} LEVEL1TREASURE {}".format(player.num_temp_failed_kills,
                                                                 player.num_temp_failed_treasure1_feedback)
    return jsonify(result)
