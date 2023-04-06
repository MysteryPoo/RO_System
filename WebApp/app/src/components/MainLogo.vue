<template>
	<section>
		<div class="content">
			<h6>Romcon</h6>
			<h6>Romcon</h6>
		</div>
	</section>
</template>

<style scoped lang="scss">
@import url("https://fonts.googleapis.com/css?family=Poppins:100,200,300,400,500,600,700,800,900");

* {
	box-sizing: border-box;
	font-family: "Poppins", sans-serif;
}

body {
	display: flex;
	background: #000;
	min-height: 100vh;
	align-items: center;
	justify-content: center;
}


.content h6 {
	color: #fff;
	position: absolute;
	transform: translate(50%, -130%);
}

.content h6:nth-child(1) {
	color: transparent;
	-webkit-text-stroke: 2px #8338ec;
}

.content h6:nth-child(2) {
	color: #c19bf5;
	animation: animate 4s ease-in-out infinite;
}

@keyframes animate {
	0%,
	100% {
		clip-path: polygon(
			0% 45%,
			16% 44%,
			33% 50%,
			54% 60%,
			70% 61%,
			84% 59%,
			100% 52%,
			100% 100%,
			0% 100%
		);
	}

	50% {
		clip-path: polygon(
			0% 60%,
			15% 65%,
			34% 66%,
			51% 62%,
			67% 50%,
			84% 45%,
			100% 46%,
			100% 100%,
			0% 100%
		);
	}
}

</style>
