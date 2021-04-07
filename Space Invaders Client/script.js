const DEFAULT_PLAYER_VELOCITY = 250.0;

// sets the player's velocity to any value between 20 and 1000 pixels per second.
// you can change the value to anything between 20 and 1000. If you exceed this range
// the velocity will be set to the closest limit.
function playerBehavior() {
	setPlayerVelocity(DEFAULT_PLAYER_VELOCITY);
}

function setInvincible(player) {
	player.makeInvincible(true);
}