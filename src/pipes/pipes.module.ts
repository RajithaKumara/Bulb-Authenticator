import { NgModule } from '@angular/core';
import { BulbNamePipe } from './bulb-name/bulb-name';
import { BulbStatePipe } from './bulb-state/bulb-state';
@NgModule({
	declarations: [
		BulbNamePipe,
		BulbStatePipe
	],
	imports: [],
	exports: [
		BulbNamePipe,
		BulbStatePipe
	]
})
export class PipesModule { }
