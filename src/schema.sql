-- todo create a migrations folder

create table if not exists "user" (
	"id" character(36) not null,
	"username" character(256) not null,
	"password" character(256) not null
);
