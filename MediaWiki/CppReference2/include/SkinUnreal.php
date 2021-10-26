<?php
/**
 * Created by TekuConcept on October 25, 2021
 */

class SkinUnreal extends SkinMustache {
    var $skinname = 'cppreference2', $stylename = 'CppReference2',
		$template = 'Cppreference2Template', $useHeadElement = true;

    public function __construct( $options = [] ) {
		parent::__construct( $options );
	}
}
