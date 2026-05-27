#include "Combat.h"

//Constructors & Destructors
Combat::Combat()
{
	//Move Values
	this->valZero = 0;
	this->valOne = 1;
	this->valTwo = 2;
	this->valThree = 3;
	this->valFour = 4;
	this->valFive = 5;

	//Attack Counters
	this->attackCounter = 0;
	this->zinAttackCounter = 0;
	this->thomAttackCounter = 0;

	//Core Bool
	this->initCombatOnce = false;
	this->reInitCombatOnce = false;
	this->combatEnd = false;

	//Hostile Forest Bools
	this->initHostileWolf = false;
	this->initHostileWalker = false;
	this->initHostileKnight = false;
	this->initHostileTreeMimic = false;
	this->initHostileLostNun = false;
	this->initHostileDecapod = false;
	this->initHostileHazeDemon = false;

	//Hostile Castle Bools
	this->initHostileJester = false;
	this->initHostileWallMimic = false;
	this->initHostileLostKnight = false;
	this->initHostilePhantom = false;
	this->initHostileEater = false;
	this->initHostileSiward = false;

	//Hostile Decay Bools
	this->initHostileLimbSplitter = false;
	this->initHostileBurrower = false;
	this->initHostileChatterMouth = false;
	this->initHostileReclus = false;
	this->initHostileTendrilAlpha = false;
	this->initHostileSpade = false;
	this->initHostileRotBeast = false;

	//Combat Control Flow Bool
	this->turnPlayer = true;
	this->turnZin = false;
	this->turnThom = false;
	this->turnHostile = false;

	this->playerAttack = false;
	this->zinAttack = false;
	this->thomAttack = false;
	this->hostileAttack = false;
	this->hostileAttackZin = false;
	this->hostileAttackThom = false;

	this->playerDead = false;
	this->zinDead = false;
	this->thomDead = false;

	//Move Selectors
	this->playerPickMove = -1;
	this->zinPickMove = 0;
	this->thomPickMove = 0;

	//Combat Rewards
	this->increase = false;
	this->expGain = 0;

	//Animation Control
	this->comTextRemoved = true;

	//Music Bools
	this->replayMusic = false;
}

Combat::~Combat()
{

}

//Core Stat Functions
void Combat::updateStats(Sprites& sprites, Player& player)
{
	//Strength Modifiers
	getPlayerStrike() = 6 + player.getStrength() + player.getSwordPower();
	getPlayerHeftyBlow() = 16 + player.getStrength() + player.getSwordPower();

	//Vitality Modifiers
	getPlayerHpMax() = 50 + player.getVitality();
	getPlayerHp() = getPlayerHpMax();

	//Fortitude Modifiers
	player.getDecayMax() = 25 + player.getFortitude();
	getIronWall() = 6 + player.getFortitude();

	//Decay Modifiers
	getPlayerDecayingSynDmg() = 5 + player.getDecay();
	getPlayerDecayingSynDef() = 5 + player.getDecay();
	getPlayerDecayedBlade() = 10 + player.getDecay();

	//Resolve Modifiers
	getZinSmite() = 6 + player.getZinResolve();
	getZinBlaze() = 20 + player.getZinResolve();
	getCrimsonFlames() = 5 + player.getZinResolve();

	//Resilience Modifiers
	getZinHpMax() = 35 + player.getZinResilience();
	getZinHp() = getZinHpMax();

	//Patience Modifiers
	getZinMend() = 8 + player.getZinPatience();
	getZinFocus() = 20 + player.getZinPatience();
}

void Combat::unlockMoves(Sprites& sprites, Player& player)
{
	switch (player.getLevel()) {
	case 5:
		//Unlock Guard
		sprites.getCombatPlayerMoves() = 1;
		sprites.text.setString("Move 'Guard' learned!");
		break;
	case 15:
		//Unlock Decayed Blade
		sprites.getCombatPlayerMoves() = 2;
		sprites.text.setString("Move 'Decayed Blade' learned!");
		break;
	case 20:
		//Unlock Hefty blow
		sprites.getCombatPlayerMoves() = 3;
		sprites.text.setString("Move 'Hefty Blow' learned!");
		break;
	case 30:
		//Unlock Decaying Synergy
		sprites.getCombatPlayerMoves() = 4;
		sprites.text.setString("Move 'Decaying Synergy' learned!");
		break;
	case 40:
		//Unlock Iron Wall
		sprites.getCombatPlayerMoves() = 5;
		sprites.text.setString("Move 'Iron Wall' learned!");
		break;
	}

	switch (player.getZinLevel()) {
	case 5:
		//Unlock Mend
		sprites.getCombatZinMoves() = 1;
		sprites.text.setString("Move 'Mend' learned!");
		break;
	case 10:
		//Unlock Vengeance
		sprites.getCombatZinMoves() = 2;
		sprites.text.setString("Move 'Vengeance' learned!");
		break;
	case 15:
		//Unlock Hellish Blaze
		sprites.getCombatZinMoves() = 3;
		sprites.text.setString("Move 'Hellish Blaze' learned!");
		break;
	case 25:
		//Unlock Focused Healing
		sprites.getCombatZinMoves() = 4;
		sprites.text.setString("Move 'Focused Healing' learned!");
		break;
	case 35:
		//Unlock Crimson Flames
		sprites.getCombatZinMoves() = 5;
		sprites.text.setString("Move 'Crimson Flames' learned!");
		break;
	}
}

void Combat::combatReward(Sprites& sprites, Player& player)
{
	if (!this->increase) {
		player.getExp() += this->expGain;
		player.getZinExp() += this->expGain;
		this->increase = true;
	}
}

//Core Combat Functions
void Combat::combatLoop(sf::RenderWindow& window, Sprites& sprites, Player& player, Animation& animate)
{
	if (!combatEnd) {
		//Combat Animations
		if (!animate.getAnimEnd()) {
			animate.pickAnimation(sprites);
		}
		//Begin combat loop initialization
		if (initCombatOnce == false) {
			this->initCombat(sprites, player, animate);
			initCombatOnce = true;
		} //Reinitialize the combat loop for each pass
		else if (reInitCombatOnce == false) {
			this->reInitCombat(sprites);
			reInitCombatOnce = true;
		}
		//Players turn
		if (!this->playerDead) {
			this->playerTurn(window, sprites, animate);
		}
		//Zin's Turn
		if (!this->zinDead) {
			this->zinTurn(sprites, animate);
		}
		if (!this->thomDead) {
			this->thomTurn(sprites, animate);
		}
		//Check if hostile is dead. If so, end combat
		if (getHostileHp() <= 0) {
			sprites.setCombatAssetsFalse();
			this->combatReward(sprites, player);
			sprites.text.setString("You have killed the " + getHostileNameNoSpc() + ". " + std::to_string(player.getExp()) + " Exp gained...");
			combatTextTime.restart();//Start timer to remove text
			this->comTextRemoved = false;//Remove text
			//Make entity viewer blank again
			sprites.getEntityViewerCounter() = -1;
			animate.getCombatAnimationLocation() = -1;
			getFatigued() = 0;
			getPlayerAtk() = 0;
			getZinAtk() = 0;
			getPlayerDef() = 0;
			getZinDef() = 0;
			getThomDef() = 0;
			getThomEnraged() = false;
			getZinFatigue() = false;
			getPlayerFatigue() = false;
			sprites.getTipBoxCounter() = -1;
			this->replayMusic = false;
			this->combatEnd = true;
			//Force hide animations
			animate.hitSprite.setPosition(sf::Vector2f(10000.0f, 10000.0f));
			animate.guardSprite.setPosition(sf::Vector2f(10000.0f, 10000.0f));
			animate.guardBrkSprite.setPosition(sf::Vector2f(10000.0f, 10000.0f));
			animate.getCombatAnimationLocation() = -1;
			animate.getCombatAnimationFrame() = -1;
			animate.getAnimEnd() = true;
		}
		//Hostiles turn
		this->hostileTurn(sprites, animate);
		//Check if player, Zin, or Thom has died
		if (getPlayerHp() <= 0 && !this->playerDead) {
			this->playerDead = true;
			sprites.text.setString("You have been left unconscious...");
			sprites.spriteText[0].setString(sprites.getPlayerName() + "     DOWN");
			sprites.spriteElements[4].setColor(sf::Color(80, 80, 80, 10));
		}
		if (getZinHp() <= 0 && !this->zinDead) {
			this->zinDead = true;
			sprites.text.setString("Zin has been left unconscious...");
			sprites.spriteText[1].setString("Zin            DOWN");
			sprites.spriteElements[3].setColor(sf::Color(80, 80, 80, 10));
		}
		if (getThomHp() <= 0 && !this->thomDead) {
			this->thomDead = true;
			sprites.text.setString("Thom has been left unconscious...");
			sprites.spriteText[5].setString("Thom          DOWN");
			sprites.spriteElements[7].setColor(sf::Color(80, 80, 80, 10));
		}
		//Check if both the player and Zin have died
		if (getPlayerHp() <= 0 && getZinHp() <= 0) {
			this->playerDeath(sprites);
			sprites.text.setString("Your party has died...");
		}
	}
}

void Combat::initCombat(Sprites& sprites, Player& player, Animation& animate)
{
	sprites.soundCombatStart.play(); //Play combat Sfx
	//this->updateStats(assets, player);
	sprites.text.setString(getHostileEncText());
	sprites.spriteText[0].setString(sprites.getPlayerName() + "     " + std::to_string(getPlayerHp()) + "/" + std::to_string(getPlayerHpMax()));
	sprites.spriteText[1].setString("Zin            " + std::to_string(getZinHp()) + "/" + std::to_string(getZinHpMax()));
	sprites.spriteText[2].setString(getHostileName() + std::to_string(getHostileHp()) + "/" + std::to_string(getHostileHpMax()));

	sprites.getPlayerCounter()++; //Load Player sprite with counter
	sprites.getZinCounter()++; //Load Zins sprite with counter
	//Load Thom Sprite with counter if he is unlocked
	if (sprites.getThomUnlocked()) {
		sprites.getThomCounter()++;
		sprites.spriteText[5].setString("Thom          " + std::to_string(getThomHp()) + "/" + std::to_string(getThomHpMax()));
	}

	sprites.setCombatAssetsTrue(); //Utilize all combat assets
	sprites.setInitMapFalse(); //Hide the map if its open
	sprites.setInitStatsFalse(); //Hide stats if open
	sprites.setInitInventoryFalse(); //Hide inventory if open

	animate.getAnimEnd() = true;//Prevent animation attempt from running at start
	getFirstAttack() = false;
}

void Combat::reInitCombat(Sprites& sprites)
{
	//Re init Hostile
	this->hostileAttack = false;
	this->hostileAttackZin = false;
	this->hostileAttackThom = false;
	this->turnHostile = false;
	sprites.getCombatCounter() = 0;
	if (!playerDead) sprites.spriteElements[4].setColor(sf::Color::White);
	if (!zinDead) sprites.spriteElements[3].setColor(sf::Color::White);
	if (!thomDead) sprites.spriteElements[7].setColor(sf::Color::White);

	//Re init thom if he is unlocked
	if (sprites.getThomUnlocked()) {
		this->turnThom = false;
		this->thomAttackCounter = 0;
		this->thomAttack = false;
		getPlayerGuarded() = false;
		sprites.getThomTurnAssets() = false;
	}

	//Re init characters if both are alive
	if (!this->playerDead && !this->zinDead) {
		//Player
		sprites.setPlayerTurnAssetsTrue();
		this->turnPlayer = true;
		this->attackCounter = 0;
		getZinGuarded() = false;
		this->playerAttack = false;
		//Zin
		sprites.setZinTurnAssetsFalse();
		this->turnZin = false;
		this->zinAttackCounter = 0;
		this->zinAttack = false;
	}//Re init zin if she is alive and player is dead
	else if (this->playerDead && !this->zinDead) {
		//Player
		sprites.setPlayerTurnAssetsFalse();
		this->turnPlayer = false;
		//Zin
		sprites.setZinTurnAssetsTrue();
		this->turnZin = true;
		this->zinAttackCounter = 0;
		this->zinAttack = false;
	}//Re init player if he is alive and zin is dead
	else if (!this->playerDead && this->zinDead) {
		//Player
		this->turnPlayer = true;
		this->attackCounter = 0;
		getZinGuarded() = false;
		this->playerAttack = false;
		//Zin
		sprites.setZinTurnAssetsFalse();
		this->turnZin = false;
	}
}

void Combat::playerDeath(Sprites& sprites)
{
	sprites.setCombatAssetsFalse();
	sprites.setPlayerDeathTrue();
}

void Combat::initEnemy(Sprites& sprites, const EnemyData& enemy)
{
	sprites.track1.pause();
	sprites.track5.play();

	sprites.getSpriteViewerCounter() = enemy.spriteViewerCounter;
	sprites.getEntityViewerCounter() = enemy.entityViewerCounter;
	sprites.getHostileCounter() = enemy.hostileCounter;

	this->combatEnd = false;
	this->initCombatOnce = false;
	this->reInitCombatOnce = false;
	this->increase = false;

	this->expGain = enemy.expGain;
	getHostileHp() = enemy.hp;
	getHostileHpMax() = enemy.hp;
	getHostileStrike() = enemy.strike;

	getHostileNameNoSpc() = enemy.nameNoSpc;
	getHostileName() = enemy.name;

	getHostileEncText() = enemy.encounterText;
	getHostileAtkPlayerText() = enemy.attackPlayerText;
	getHostileAtkZinText() = enemy.attackZinText;
	getHostileAtkZinBlkText() = enemy.attackZinBlockedText;

	getPlayerSlashAtkText() = enemy.playerSlashText;
	getPlayerGuardAtkText() = enemy.playerGuardText;
	getPlayerDecayAtkText() = enemy.playerDecayText;

	getZinSmiteAtkText() = enemy.zinSmiteText;
	getZinMendAtkText() = enemy.zinMendText;
	getZinVengeanceAtkText() = enemy.zinVengeanceText;
}

//Combat Functions
void Combat::playerTurn(sf::RenderWindow& window, Sprites& sprites, Animation& animate)
{
	if (this->turnPlayer == true) {
		switch (this->attackCounter) {
		case 0:
			if (!getPlayerFatigue()) {
				//Check if the player is fatigued, if not continue turn as normal
				sprites.setPlayerTurnAssetsTrue();
				sprites.text.setString(getPlayerTurnText());
			}
			else if (getPlayerFatigue()) {
				sprites.text.setString("You are forced to take a moment to breathe after your last attack...");
			}
			break;
		case 1:
			if (!this->playerAttack && !getPlayerFatigue()) {
				this->playerSelectMove(sprites, animate);
				this->playerAttack = true;
			}
			else if (getPlayerFatigue() && !this->playerAttack) {
				getPlayerFatigue() = false;
				sprites.getPlayerTurnAssets() = false;
				this->attackCounter = 2;
				this->playerAttack = true;
			}
			break;
		case 2:
			this->turnPlayer = false;
			if (sprites.getThomUnlocked()) {
				//Check who's alive and set it to their turn
				if (!this->zinDead) {
					this->turnZin = true;
				}
				else if (this->zinDead && !this->thomDead) {
					this->turnThom = true;
				}
				else if (this->zinDead && this->thomDead) {
					this->turnHostile = true;
				}
			} else if (!sprites.getThomUnlocked()) {
				if (!this->zinDead) {
					this->turnZin = true;
				}
				else if (this->zinDead) {
					this->turnHostile = true;
				}
			}
			break;
		}
	}
}

void Combat::zinTurn(Sprites& sprites, Animation& animate)
{
	if (this->turnZin == true) {
		switch (this->zinAttackCounter) {
		case 0:
			if (!getZinFatigue()) {
				//Check if Zin is fatigued, if not continue turn as normal
				sprites.text.setString(getZinTurnText());
				sprites.setZinTurnAssetsTrue();
			}
			else if (getZinFatigue()) {
				sprites.text.setString("Zin can hardly stand after her last attack... She takes a moment to rest...");
			}
			break;
		case 1:
			//Zin Attacks Hostile
			if (!getZinFatigue() && !this->zinAttack) {
				this->zinSelectMove(sprites, animate);
				this->zinAttack = true;
			}
			else if (getZinFatigue() && !this->zinAttack) {
				getFatigued()--;
				if (getFatigued() == 0) {
					getZinFatigue() = false;
					sprites.getZinTurnAssets() = false;
					sprites.text.setString("Zin finally catches her breath, ready to fight once more...");
				}
				this->zinAttackCounter = 2;
				this->zinAttack = true;
			}
			break;
		case 2:
			this->turnZin = false;

			if (sprites.getThomUnlocked() && !this->thomDead) {
				this->turnThom = true;
			}
			else {
				this->turnHostile = true;
			}
			break;
		}
	}
}

void Combat::thomTurn(Sprites& sprites, Animation& animate)
{
	if (this->turnThom == true) {
		switch (this->thomAttackCounter) {
		case 0:
			if (!getThomEnraged()) {
				sprites.text.setString("Thom prepares his next move");
				sprites.setThomTurnAssetsTrue();
			}
			else if (getThomEnraged()) {
				this->thomAttackCounter = 1;
			}
			break;
		case 1:
			//Thoms turn
			if (!this->thomAttack && !getThomEnraged()) {
				this->thomSelectMove(sprites, animate);
				this->thomAttack = true;
			}
			else if (this->thomAttack == false && getThomEnraged()) {
				sprites.text.setString("Thom still stands in a state of pure rage...");
				getEnraged()--;
				if (getEnraged() == 0) {
					getThomEnraged() = false;
					sprites.getThomCounter() = 0;
					sprites.text.setString("Thom snaps out of his rage!");
				}
				this->thomAttack = true;
			}
			break;
		case 2:
			this->turnThom = false;
			this->turnHostile = true;
			break;
		}
	}
}

void Combat::hostileTurn(Sprites& sprites, Animation& animate)
{
	if (this->turnHostile == true) {

		// Only attack once per hostile turn
		if (!this->hostileAttack) {

			std::vector<int> targets;

			if (!this->playerDead)
				targets.push_back(0);

			if (!this->zinDead)
				targets.push_back(1);

			if (sprites.getThomUnlocked() && !this->thomDead)
				targets.push_back(2);

			// Nobody valid to attack
			if (targets.empty()) {
				this->reInitCombatOnce = false;
				this->turnHostile = false;
				return;
			}

			int pickedTarget = targets[static_cast<size_t>(rand() % targets.size())];

			switch (pickedTarget) {
			case 0:
				if (!getPlayerGuarded()) {
					int damage = std::max(0, getHostileStrike() - getPlayerDef());
					getPlayerHp() = std::max(0, getPlayerHp() - damage);
					getPlayerDef() = std::max(0, getPlayerDef() - getHostileStrike());
					sprites.soundCom.play();
					sprites.spriteText[0].setString(sprites.getPlayerName() + "     " + std::to_string(getPlayerHp()) + "/" + std::to_string(getPlayerHpMax()));
					sprites.text.setString(getHostileAtkPlayerText());
					animate.getCombatAnimationLocation() = 1;
					animate.getAnimation() = 0;
				}
				else {
					sprites.text.setString(getHostileAtkPlayerBlkText());
					sprites.soundPlayerGuarded.play();
					animate.getCombatAnimationLocation() = 1;
					animate.getAnimation() = 3;
				}
				break;

			case 1:
				if (!getZinGuarded()) {
					int damage = std::max(0, getHostileStrike() - getZinDef());
					getZinHp() = std::max(0, getZinHp() - damage);
					getZinDef() = std::max(0, getZinDef() - getHostileStrike());
					sprites.soundCom.play();
					sprites.spriteText[1].setString("Zin            " + std::to_string(getZinHp()) + "/" + std::to_string(getZinHpMax()));
					sprites.text.setString(getHostileAtkZinText());
					animate.getCombatAnimationLocation() = 2;
					animate.getAnimation() = 0;
				}
				else {
					sprites.soundGuarded.play();
					sprites.text.setString(getHostileAtkZinBlkText());
					animate.getCombatAnimationLocation() = 2;
					animate.getAnimation() = 3;
				}
				break;

			case 2:
				int damage = std::max(0, getHostileStrike() - getThomDef());
				getThomHp() = std::max(0, getThomHp() - damage);
				getThomDef() = std::max(0, getThomDef() - getHostileStrike());
				sprites.soundCom.play();
				sprites.spriteText[5].setString("Thom          " + std::to_string(getThomHp()) + "/" + std::to_string(getThomHpMax()));
				sprites.text.setString("The hostile strikes Thom!");
				animate.getCombatAnimationLocation() = 3;
				animate.getAnimation() = 0;
				break;
			}

			animate.getAnimEnd() = false;
			this->hostileAttack = true;
			sprites.getCombatCounter() = 0;
		}
		else {
			if (sprites.getCombatCounter() >= 1) {
				this->turnHostile = false;
				this->reInitCombatOnce = false;
				sprites.getCombatCounter() = 0;
			}
		}
	}
}

//Combat Pick Attacks
void Combat::pickMove(sf::RenderWindow& window, Sprites& sprites)
{
	if (this->turnPlayer) {
		this->pickMoveFunc(window, sprites.combatRect[0], this->attackCounter, this->playerPickMove, this->valZero);
		this->pickMoveFunc(window, sprites.combatRect[1], this->attackCounter, this->playerPickMove, this->valOne);
		this->pickMoveFunc(window, sprites.combatRect[2], this->attackCounter, this->playerPickMove, this->valTwo);
		this->pickMoveFunc(window, sprites.combatRect[3], this->attackCounter, this->playerPickMove, this->valThree);
		this->pickMoveFunc(window, sprites.combatRect[4], this->attackCounter, this->playerPickMove, this->valFour);
		this->pickMoveFunc(window, sprites.combatRect[5], this->attackCounter, this->playerPickMove, this->valFive);
	}
	if (this->getTurnZin()) {
		this->pickMoveFunc(window, sprites.combatRect[6], this->zinAttackCounter, this->zinPickMove, this->valZero);
		this->pickMoveFunc(window, sprites.combatRect[7], this->zinAttackCounter, this->zinPickMove, this->valOne);
		this->pickMoveFunc(window, sprites.combatRect[8], this->zinAttackCounter, this->zinPickMove, this->valTwo);
		this->pickMoveFunc(window, sprites.combatRect[9], this->zinAttackCounter, this->zinPickMove, this->valThree);
		this->pickMoveFunc(window, sprites.combatRect[10], this->zinAttackCounter, this->zinPickMove, this->valFour);
		this->pickMoveFunc(window, sprites.combatRect[11], this->zinAttackCounter, this->zinPickMove, this->valFive);
	}
	if (this->getTurnThom()) {
		this->pickMoveFunc(window, sprites.combatRect[12], this->thomAttackCounter, this->thomPickMove, this->valZero);
		this->pickMoveFunc(window, sprites.combatRect[13], this->thomAttackCounter, this->thomPickMove, this->valOne);
	}
}

void Combat::pickMoveFunc(sf::RenderWindow& window, sf::RectangleShape& inputRect, int& counter, int& moveChar, int& moveVal)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
	//Player Combat Buttons Functionality
	if (counter == 0 && inputRect.getGlobalBounds().contains(mousePosF)) {
		counter++;
		moveChar = moveVal;
	}
}

void Combat::playerSelectMove(Sprites& sprites, Animation& animate)
{
	switch (this->playerPickMove) {
	case 0:
		slash(sprites, animate);
		break;
	case 1:
		guard(sprites, animate);
		break;
	case 2:
		decayBlade(sprites, animate);
		break;
	case 3:
		heftyBlow(sprites, animate);
		break;
	case 4:
		decaySynergy(sprites, animate);
		break;
	case 5:
		ironWall(sprites, animate);
		break;
	}
}

void Combat::zinSelectMove(Sprites& sprites, Animation& animate)
{
	switch (this->zinPickMove) {
	case 0:
		smite(sprites, animate);
		break;
	case 1:
		mend(sprites, animate);
		break;
	case 2:
		vengeance(sprites, animate);
		break;
	case 3:
		hellBlaze(sprites, animate);
		break;
	case 4:
		focusHeal(sprites, animate);
		break;
	case 5:
		crimsonFlames(sprites, animate);
		break;
	}
	
}

void Combat::thomSelectMove(Sprites& sprites, Animation& animate)
{
	switch (this->thomPickMove) {
	case 0:
		barrier(sprites, animate);
		break;
	case 1:
		enrage(sprites, animate);
		break;
	}
}

//Combat Init Forest Hostiles
void Combat::initWolf(Sprites& sprites)
{
	if (!this->initHostileWolf) {
		EnemyData enemy{
			0, 9, 1, -1,
			0, 45, 30, 6,
			"Wolf",
			"Wolf	    ",
			"A wolf leaps out of the bushes, ambushing you!",
			"The wolf lunges forwards, biting you!",
			"The wolf jumps towards Zin, biting her!",
			"The wolf leaps in the air towards Zin, however you block it just in time!",
			"You slash at the wolf, hitting it!",
			"You watch the wolfs movements and prepare yourself to defend Zin...",
			"You slash yourself open with your sword, using the decay in your blood to strike the wolf!",
			"Zin places her hands together and creates a bolt of lightning, striking the wolf!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the wolf!"
		};

		initEnemy(sprites, enemy);
		this->initHostileWolf = true;
	}
}

void Combat::initDecayWalker(Sprites& sprites)
{
	if (!this->initHostileWalker) {
		EnemyData enemy{
			0, 1, 2, -1,
			0, 60, 40, 5,
			"Decay Walker",
			"Decay Walker ",
			"A horrific creature appears before you, threatening your life!",
			"The walking pile of decay swings at you, striking you!",
			"The walker jumps towards Zin, hitting her!",
			"The walker leaps towards Zin, however you deflect the attack just in time!",
			"You slash at the vile creature, black ooze",
			"You watch the walkers movements and prepare yourself to defend Zin...",
			"You slash yourself open with your sword, using the decay in your blood to strike the walker!",
			"Zin places her hands together and creates a bolt of lightning, smiting the unholy creature!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the walker!"
		};

		initEnemy(sprites, enemy);
		this->initHostileWalker = true;
	}
}

void Combat::initHostileTree(Sprites& sprites)
{
	if (!this->initHostileTreeMimic) {
		EnemyData enemy{
			0, 5, 4, -1,
			0, 90, 70, 3,
			"Tree Mimic",
			"Tree Mimic ",
			"A tree shifts into a horrifying creature, revealing itself and lunging at you!",
			"The mimic strikes you with a spiked branch!",
			"The mimic launches at Zin, striking her with a branch!",
			"The mimic rushes towards Zin, however you deflect its strike just in time!",
			"You slash at the mimic, slowly chipping away at the wood holding its form.",
			"You pay close attention to the mimic, preparing to defend Zin.",
			"You slash yourself open with your sword, using the decay in your blood to strike the mimic!",
			"Zin places her hands together and creates a bolt of lightning, smiting the mimic!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the mimic!"
		};

		initEnemy(sprites, enemy);
		this->initHostileTreeMimic = true;
	}
}

void Combat::initDecayKnight(Sprites& sprites)
{
	if (!this->initHostileKnight) {
		EnemyData enemy{
			0, 10, 3, -1,
			0, 120, 110, 9,
			"Decay Knight",
			"Decay Knight ",
			"You're ambushed by a horrifying knight rotting from decay!",
			"The knight lunges forwards with precision, striking you!",
			"The knight swings his sword at Zin, striking her!",
			"The knight lunges his sword towards Zin, however you deflect it just on time!",
			"You slash at the knight, attempting to pierce his armor!",
			"You watch the knights movements and prepare yourself to defend Zin...",
			"You slash yourself open with your sword, using the decay in your blood to strike the knight!",
			"Zin places her hands together and creates a bolt of lightning, smiting the knight!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the knight!"
		};

		initEnemy(sprites, enemy);
		this->initHostileKnight = true;
	}
}

void Combat::initLostNun(Sprites& sprites)
{
	if (!this->initHostileLostNun) {
		EnemyData enemy{
			0, -1, 5, -1,
			0, 180, 180, 12,
			"Lost Nun",
			"Lost Nun ",
			"The woman steps forwards, revealing the sheer spread of her decay. She readies to attack you.",
			"The nun launches forwards as she spews out rotting decay, brutally striking you!",
			"The nun targets Zin, hitting her with horrifying amounts of molten decay!",
			"The nun targets Zin launching more decay at her, however you deflect it just on time!",
			"You slash at the nun, the woman screaming out in pain as you attack.",
			"You watch the nuns movements preparing yourself to defend Zin at any cost...",
			"You slash yourself open with your sword, using the decay in your blood to strike the nun!",
			"Zin places her hands together and creates a bolt of lightning, smiting the nun!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the nun!"
		};

		initEnemy(sprites, enemy);
		this->initHostileLostNun = true;
	}
}

void Combat::initDecapod(Sprites& sprites)
{
	if (!this->initHostileDecapod) {
		EnemyData enemy{
			0, 8, 6, -1,
			0, 110, 90, 10,
			"Abyssal Decapod",
			"Abyssal Decapod ",
			"The horrifying abomination stands before you as its face wriggles with tentacles...",
			"The horrifying abomination grabs ahold of you and strikes you violently!",
			"The creature spits acid directly at Zin, hitting her with it!",
			"The abomination attempts to spit at Zin with acid, however you block it with your sword just in time!",
			"You slash at the decapod, the creature screaming in pain!",
			"You watch the decapods movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, using the decay in your blood to strike the decapod!",
			"Zin places her hands together and creates a bolt of lightning, smiting the decapod!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the decapod!"
		};

		initEnemy(sprites, enemy);
		this->initHostileDecapod = true;
	}
}

void Combat::initHazeDemon(Sprites& sprites)
{
	if (!this->initHostileHazeDemon) {
		EnemyData enemy{
			0, 11, 7, -1,
			0, 110, 90, 10,
			"Haze Demon",
			"Haze Demon ",
			"The horrifying abomination stands before you as it creatres a strange auora...",
			"The horrifying abomination grabs ahold of you and strikes you violently!",
			"The creature splices the air in front Zin, hitting her with the airwaves!",
			"The abomination attempts to split the air towards Zin, however you block it just in time!",
			"You slash at the demon, the unholy entity screaming in pain!",
			"You watch the demons movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, using the decay in your blood to strike the demon!",
			"Zin places her hands together and creates a bolt of lightning, smiting the demon!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the demon!"
		};

		initEnemy(sprites, enemy);
		this->initHostileHazeDemon = true;
	}
}

void Combat::initCourtJester(Sprites& sprites)
{
	if (!this->initHostileJester) {
		EnemyData enemy{
			0, 12, 8, -1,
			1, 150, 140, 14,
			"Court Jester",
			"Court Jester ",
			"The jester stands with a horrifying grin...",
			"The jester stabs you with a small dagger, brutally injuring you!",
			"The jester sprints towards Zin, stabbing her with his knife!",
			"The jester runs towards Zin with a disgusting smile and a knife in his hand, however you block the knife just in time!",
			"You slash at the jester, the clown smiling in pain!",
			"You watch the jesters movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the jester, causing him to yell in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the jester!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the jester!"
		};

		initEnemy(sprites, enemy);
		this->initHostileJester = true;
	}
}

void Combat::initWallMimic(Sprites& sprites)
{
	if (!this->initHostileWallMimic) {
		EnemyData enemy{
			0, 13, 9, -1,
			1, 165, 170, 13,
			"Wall Mimic",
			"Wall Mimic ",
			"The walls begin to shift as the face appears before you.",
			"The walls shift as the mimic launches towards you, striking you!",
			"The mimic targets Zin, striking her with extreme force!",
			"The mimic attempts to bite Zin, however you tackle her out of the way just in time!",
			"You slash at the mimic, the walls echoing back with screams!",
			"You watch the movements of the walls carefully, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the walls and ceiling striking the mimic!",
			"Zin places her hands together and creates a bolt of lightning that obliterates the ceiling and strikes the mimic!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the walls!"
		};

		initEnemy(sprites, enemy);
		this->initHostileWallMimic = true;
	}
}

void Combat::initLostKnight(Sprites& sprites)
{
	if (!this->initHostileLostKnight) {
		EnemyData enemy{
			0, 14, 10, -1,
			1, 220, 240, 16,
			"Lost Knight",
			"Lost Knight ",
			"The knight stands in protest, ready to fight to the death...",
			"The knight stabs you with his sword!",
			"The knight dashes towards Zin, slightly hesitanting due to her being a child. Despite this, he still strikes her!",
			"The knight dashes towards Zin slashing at her, however you block the blade just in time!",
			"You slash at the knight, the knight gritting his teeth in pain!",
			"You watch the knights movements, preparing yourself to defend Zin from his attacks...",
			"You slash yourself open with your sword, spraying your decayed blood into the knights helmet! He begins coughing and choking in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the knight!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the knight!"
		};

		initEnemy(sprites, enemy);
		this->initHostileLostKnight = true;
	}
}

void Combat::initPhantom(Sprites& sprites)
{
	if (!this->initHostilePhantom) {
		EnemyData enemy{
			0, 15, 11, -1,
			1, 170, 160, 12,
			"Phantom",
			"Phantom ",
			"The phantom phases into existence right before you, charging...",
			"The phantom appears right before you, striking you before you can react!",
			"The phantom vanishes before reappearing next to Zin, using the opprotunity to strike her!",
			"The phantom runs towards Zin with a disgusting smirk, however you block the attack just in time!",
			"You slash at the phantom, its physical form being caught by your blade!",
			"You watch the phantoms movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the phantom!",
			"Zin places her hands together and creates a bolt of lightning, smiting the phantom!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the phantom!"
		};

		initEnemy(sprites, enemy);
		this->initHostilePhantom = true;
	}
}

void Combat::initSkinEater(Sprites& sprites)
{
	if (!this->initHostileEater) {
		EnemyData enemy{
			0, 16, 12, -1,
			1, 210, 260, 10,
			"Skin Eater",
			"Skin Eater ",
			"The skin eater stands with a horrifying face, locking eyes with you...",
			"The skin eater attempts to bite you but only grazes you!",
			"The skin eater sprints towards Zin, lunging at her and striking her!",
			"The skin eater runs towards Zin, however you pull Zin out of the way just in time!",
			"You slash at the skin eater, the abomination screaming in pain!",
			"You watch the skin eaters movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the skin eater, causing it to yell in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the skin eater!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the skin eater!"
		};

		initEnemy(sprites, enemy);
		this->initHostileEater = true;
	}
}

void Combat::initSiward(Sprites& sprites)
{
	if (!this->initHostileSiward) {
		EnemyData enemy{
			-1, -1, -1, -1,
			3, 600, 700, 18,
			"Siward",
			"Siward ",
			"Siward stands with decay dripping from the front of his helmet...",
			"Siward slashes his sword at you with the precision of a true knight!",
			"Siward slashes towards Zin, brutally striking her!",
			"Siward sends his sword towards Zin with great might, however you deflect it just in time!",
			"You slash Siward, the knight gritting his teeth in pain!",
			"You watch Siward's movements, preparing to defend Zin from his brutal strikes...",
			"You slash yourself open with your sword, spraying your decayed blood over Siward, though it seems to have little effect...",
			"Zin places her hands together reluctantly and creates a bolt of lightning, smiting Siward!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into Siward!"
		};

		initEnemy(sprites, enemy);
		this->initHostileSiward = true;
	}
}

void Combat::initLimbSplitter(Sprites& sprites)
{
	if (!this->initHostileLimbSplitter) {
		EnemyData enemy{
			0, 17, 13, -1,
			2, 300, 360, 22,
			"Limb Splitter",
			"Limb Splitter ",
			"The limb splitter stares you down, waiting for its meal...",
			"The limb splitter strikes you with one of its ligaments!",
			"The limb splitter catches Zin off guard, striking her with one of its arms!",
			"The limb splitter attempts to impale Zin with one of its arms, however you tackle her out of the way just in time!",
			"You slash at the limb splitter, the abomination screaming in pain!",
			"You watch the limb splitters ligaments, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the limb splitter, causing it to screech in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the limb splitter!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the limb splitter!"
		};

		initEnemy(sprites, enemy);
		this->initHostileLimbSplitter = true;
	}
}

void Combat::initBurrower(Sprites& sprites)
{
	if (!this->initHostileBurrower) {
		EnemyData enemy{
			0, 18, 14, -1,
			2, 270, 320, 18,
			"Burrower",
			"Burrower ",
			"The burrower quickly rushes towards you, ready to attack...",
			"The burrower crashes up from the ground, striking you with its spiked body!",
			"The burrower catches Zin off guard, striking with one of its spiked quills!",
			"The burrower bursts from the wall, attempting to consume Zin whole, however you tackle her out of the way just in time!",
			"You slash at the burrower, the abomination bleeding across the crimson floor!",
			"You listen to the burrowers movements in the walls, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the burrower, causing it to scream out in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the burrower!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the burrower!"
		};

		initEnemy(sprites, enemy);
		this->initHostileBurrower = true;
	}
}

void Combat::initChatterMouth(Sprites& sprites)
{
	if (!this->initHostileChatterMouth) {
		EnemyData enemy{
			0, 19, 15, -1,
			2, 240, 280, 15,
			"Chatter Mouth",
			"Chatter Mouth ",
			"The chatter mouth quickly rushes towards you, ready to attack...",
			"The chatter mouth uses its wide body to swing around the limited cave space, smashing you into the wall!",
			"The chatter mouths body strikes Zin as it swings it violently!",
			"The chatter mouth attempts to drag Zin into its serrated mouth, however you firmly grab ahold of Zin's arm and pull her out of its grasp!",
			"You slash at the chatter mouth, the abomination bleeding across the crimson floor!",
			"You watch the chatter mouths movements, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the chatter mouth, causing it to scream out in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the chatter mouth!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the chatter mouth!"
		};

		initEnemy(sprites, enemy);
		this->initHostileChatterMouth = true;
	}
}

void Combat::initReclus(Sprites& sprites)
{
	if (!this->initHostileReclus) {
		EnemyData enemy{
			0, 20, 16, -1,
			2, 220, 260, 15,
			"Reclus",
			"Reclus ",
			"The reclus moves quickly rushing forwards ready to attack...",
			"The reclus smashes you with its hand into the wall!",
			"The reclus drags its hand across the floor, striking Zin!",
			"The reclus mouth attempts to drag Zin into its serrated mouth, however you firmly grab ahold of Zin's arm and pull her out of its grasp!",
			"You slash at the reclus, the abomination bleeding across the crimson floor!",
			"You watch the reclus carefully, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the reclus, causing it to scream out in pain!",
			"Zin places her hands together and creates a bolt of lightning, smiting the reclus!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the reclus!"
		};

		initEnemy(sprites, enemy);
		this->initHostileReclus = true;
	}
}

void Combat::initTendrilAlpha(Sprites& sprites)
{
	if (!this->initHostileTendrilAlpha) {
		EnemyData enemy{
			0, 21, 17, -1,
			2, 290, 350, 18,
			"Tendril Alpha",
			"Tendril Alpha ",
			"The abomination moves quickly rushing forwards ready to attack...",
			"The abomination smashes one of its tendrils into you!",
			"The abomination strikes Zin with one of its tendrils!",
			"The alpha attempts to swarm Zin with its tendrils, however you jump in between each tendril slicing them away!",
			"You slash at the alpha, its body squirming in pain!",
			"You watch the abominations tendrils carefully, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the alpha, causing it to quiver!",
			"Zin places her hands together and creates a bolt of lightning, smiting the alpha!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the alpha!"
		};

		initEnemy(sprites, enemy);
		this->initHostileTendrilAlpha = true;
	}
}

void Combat::initSpade(Sprites& sprites)
{
	if (!this->initHostileSpade) {
		EnemyData enemy{
			-1, -1, -1, 6,
			4, 500, 600, 22,
			"Spade",
			"Spade ",
			"Spade steps forwards, a horrifying look in her eyes as she's practically inhuman...",
			"Spade slams you with a horrifying decaying tentacle!",
			"Spade strikes Zin with one of her decaying tendrils!",
			"Spade attempts to slice Zin in half with a crude sharpened bone, however you deflect it just in time!",
			"You slash at Spade, her cackling back in bliss!",
			"You watch Spades movements, preparing yourself to protect Zin at all costs...",
			"You slash yourself open with your sword, spraying your decayed blood over Spade as she slowly locks eyes with you!",
			"Zin places her hands together and creates a bolt of lightning, smiting Spade!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into Spade!"
		};

		initEnemy(sprites, enemy);
		this->initHostileSpade = true;
	}
}

void Combat::initRotBeast(Sprites& sprites)
{
	if (!this->initHostileRotBeast) {
		EnemyData enemy{
			0, -1, -1, -1,
			4, 900, 900, 24,
			"The Rot Beast",
			"The Rot Beast ",
			"The horrifying beast moves quickly rushing forwards ready to attack...",
			"The beast smashes you with one of its oversized arms!",
			"The abomination strikes Zin with a powerful blast of decay!",
			"The rotting beast attempts to smash Zin with its arm, however you tackle Zin out of the way moments before death!",
			"You slash at the beast, a horrifying bellow coming from it!",
			"You watch the beast carefully, preparing yourself to defend Zin from its attacks...",
			"You slash yourself open with your sword, spraying your decayed blood over the rotting beast, causing it to quiver!",
			"Zin places her hands together and creates a bolt of lightning, smiting the beast!",
			"Zin slowly moves her arms outwards, casting a green aura around you and herself, restoring health and slowly burning away the decay...",
			"Zin uses the blood spilled from your body to create blades made of blood, casting them into the beast!"
		};

		initEnemy(sprites, enemy);
		this->initHostileRotBeast = true;
	}
}