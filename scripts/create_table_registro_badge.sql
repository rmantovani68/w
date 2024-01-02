create table badge_registry 
(
	id serial,
	read_time timestamp default now(),
	badge_id text,
	operatore_id text,
	terminal_id integer,
	isola_id integer,
	settore_id integer,
	stato_lettura text default ''
);
