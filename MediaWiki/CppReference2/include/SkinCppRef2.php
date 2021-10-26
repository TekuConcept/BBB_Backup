<?php
/**
 * Created by TekuConcept on October 25, 2021
 */

class SkinCppRef2 extends SkinTemplate {
    var $skinname = 'cppreference2',
        $stylename = 'CppReference2',
		$template = 'CppRef2Template',
        $useHeadElement = true;

    public function __construct( $options = [] ) {
        // $options['template'] = 'Cppreference2Template'
		parent::__construct( $options );
	}

    // public function generateHTML() {
    //     // $this->setupTemplateContext();
    //     // $out = $this->getOutput();
    //     // $html = $out->headElement( $this );

    //     // $temp = new CppRef2Template();
    //     // $temp->set( 'skin' , $this );

    //     // $html .= $temp->execute();
    //     // $html .= $out->tailElement( $this );
	// 	// return $html;
    // }

    public function initPage( OutputPage $out ) {
		global $wgLocalStylePath, $wgRequest;
		parent::initPage( $out );
		$out->addModules( 'skins.cppreference2' );
	}

    /**
	 * Load skin and user CSS files in the correct order
	 * @param $out OutputPage object
	 */
	function setupSkinUserCss( OutputPage $out ){
		parent::setupSkinUserCss( $out );
		$out->addModuleStyles( 'skins.cppreference2' );
	}
}

class CppRef2Template extends BaseTemplate {
    public function execute() {
        global $wgLang, $wgVectorUseIconWatch;

        // $this->data['skin'] = $skin;
		// $this->skin = $this->data['skin'];

		// Build additional attributes for navigation urls
		//$nav = $this->getSkin()->buildNavigationUrls();
		$nav = $this->data['content_navigation'];

		if ( $wgVectorUseIconWatch ) {
			$mode = $this->getSkin()->getTitle()->userIsWatching() ? 'unwatch' : 'watch';
			if ( isset( $nav['actions'][$mode] ) ) {
				$nav['views'][$mode] = $nav['actions'][$mode];
				$nav['views'][$mode]['class'] = rtrim( 'icon ' . $nav['views'][$mode]['class'], ' ' );
				$nav['views'][$mode]['primary'] = true;
				unset( $nav['actions'][$mode] );
			}
		}

		$xmlID = '';
		foreach ( $nav as $section => $links ) {
			foreach ( $links as $key => $link ) {
				if ( $section == 'views' && !( isset( $link['primary'] ) && $link['primary'] ) ) {
					$link['class'] = rtrim( 'collapsible ' . $link['class'], ' ' );
				}

				$xmlID = isset( $link['id'] ) ? $link['id'] : 'ca-' . $xmlID;
				$nav[$section][$key]['attributes'] =
					' id="' . Sanitizer::escapeIdForLink( $xmlID ) . '"';
				if ( $link['class'] ) {
					$nav[$section][$key]['attributes'] .=
						' class="' . htmlspecialchars( $link['class'] ) . '"';
					unset( $nav[$section][$key]['class'] );
				}
				if ( isset( $link['tooltiponly'] ) && $link['tooltiponly'] ) {
					$nav[$section][$key]['key'] =
						Linker::tooltip( $xmlID );
				} else {
					$nav[$section][$key]['key'] =
						Xml::expandAttributes( Linker::tooltipAndAccesskeyAttribs( $xmlID ) );
				}
			}
		}
		$this->data['namespace_urls'] = $nav['namespaces'];
		$this->data['view_urls'] = $nav['views'];
		$this->data['action_urls'] = $nav['actions'];
		$this->data['variant_urls'] = $nav['variants'];

		// Reverse horizontally rendered navigation elements
		if ( $wgLang->isRTL() ) {
			$this->data['view_urls'] =
				array_reverse( $this->data['view_urls'] );
			$this->data['namespace_urls'] =
				array_reverse( $this->data['namespace_urls'] );
		}
		// Output HTML Page
		$this->html( 'headelement' );

        $this->renderHeader();
        $this->renderMainBody();
        $this->renderMainFooter();
    }

    // ========================================================================

    // ========================================================================

    private function renderHeader() {
        global $Cppreference2SkinRootLink;
        $root_link = '/';
        if (isset($Cppreference2SkinRootLink))
        { $root_link = htmlspecialchars($Cppreference2SkinRootLink); }

        ?>
        <!-- header -->
        <div id="mw-head" class="noprint">
        <div id="cpp-head-first-base">
                <div id="cpp-head-first">
                    <h5><a href="<?php echo $root_link; ?>">
                        <?php global $wgSitename; echo $wgSitename;?>
                    </a></h5>
                    <div id="cpp-head-search">
                        <?php $this->renderNavigation( 'SEARCH' ); ?>
                    </div>
                    <div id="cpp-head-personal">
                        <?php $this->renderNavigation( 'PERSONAL' ); ?>
                    </div>
                </div>
            </div>
            <div id="cpp-head-second-base">
                <div id="cpp-head-second">
                    <div id="cpp-head-tools-left">
                        <?php $this->renderNavigation( array( 'NAMESPACES', 'VARIANTS' ) ); ?>
                    </div>
                    <div id="cpp-head-tools-right">
                        <?php $this->renderNavigation( array( 'VIEWS', 'ACTIONS' ) ); ?>
                    </div>
                </div>
            </div>
        </div>
        <!-- /header -->
        <?php
    }

	private function renderNavigation( $elements ) {
		global $wgVectorUseSimpleSearch, $wgVectorShowVariantName, $wgUser, $wgLang;

		// If only one element was given, wrap it in an array, allowing more
		// flexible arguments
		if ( !is_array( $elements ) ) { $elements = array( $elements ); }
        // If there's a series of elements, reverse them when in RTL mode
        elseif ( $wgLang->isRTL() ) { $elements = array_reverse( $elements ); }

        // Render elements
		foreach ( $elements as $name => $element ) {
			echo "\n<!-- {$name} -->\n";
			switch ( $element ) {
            case 'NAMESPACES':
                ?>
                <div id="p-namespaces" class="vectorTabs<?php if ( count( $this->data['namespace_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
                    <h5><?php $this->msg( 'namespaces' ) ?></h5>
                    <ul<?php $this->html( 'userlangattributes' ) ?>>
                        <?php foreach ( $this->data['namespace_urls'] as $link ): ?>
                            <li <?php echo $link['attributes'] ?>><span><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php echo htmlspecialchars( $link['text'] ) ?></a></span></li>
                        <?php endforeach; ?>
                    </ul>
                </div>
                <?php
                break;
            case 'VARIANTS':
                ?>
                <div id="p-variants" class="vectorMenu<?php if ( count( $this->data['variant_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
                    <?php if ( $wgVectorShowVariantName ): ?>
                        <h4>
                        <?php foreach ( $this->data['variant_urls'] as $link ): ?>
                            <?php if ( stripos( $link['attributes'], 'selected' ) !== false ): ?>
                                <?php echo htmlspecialchars( $link['text'] ) ?>
                            <?php endif; ?>
                        <?php endforeach; ?>
                        </h4>
                    <?php endif; ?>
                    <h5><span><?php $this->msg( 'variants' ) ?></span><a href="#"></a></h5>
                    <div class="menu">
                        <ul<?php $this->html( 'userlangattributes' ) ?>>
                            <?php foreach ( $this->data['variant_urls'] as $link ): ?>
                                <li<?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php echo htmlspecialchars( $link['text'] ) ?></a></li>
                            <?php endforeach; ?>
                        </ul>
                    </div>
                </div>
                <?php
                break;
            case 'VIEWS':
                ?>
                <div id="p-views" class="vectorTabs<?php if ( count( $this->data['view_urls'] ) == 0 ) { echo ' emptyPortlet'; } ?>">
                    <h5><?php $this->msg('views') ?></h5>
                    <ul<?php $this->html('userlangattributes') ?>>
                        <?php foreach ( $this->data['view_urls'] as $link ): ?>
                            <li<?php echo $link['attributes'] ?>><span><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php
                                // $link['text'] can be undefined - bug 27764
                                if ( array_key_exists( 'text', $link ) ) {
                                    echo array_key_exists( 'img', $link ) ?  '<img src="' . $link['img'] . '" alt="' . $link['text'] . '" />' : htmlspecialchars( $link['text'] );
                                }
                                ?></a></span></li>
                        <?php endforeach; ?>
                    </ul>
                </div>
                <?php
                break;
            case 'ACTIONS':
                ?>
                <div id="p-cactions" class="vectorMenu<?php if ( count( $this->data['action_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
                    <h5><span><?php $this->msg( 'actions' ) ?></span><a href="#"></a></h5>
                    <div class="menu">
                        <ul<?php $this->html( 'userlangattributes' ) ?>>
                            <?php foreach ( $this->data['action_urls'] as $link ): ?>
                                <li<?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php echo htmlspecialchars( $link['text'] ) ?></a></li>
                            <?php endforeach; ?>
                        </ul>
                    </div>
                </div>
                <?php
                break;
            case 'PERSONAL':
                $personal_urls = $this->get( 'personal_urls' );
                if ( count( $personal_urls ) > 0 ) {
                    $personalTools = $this->getSkin()->getStructuredPersonalTools();
                    
                    $item = reset($personalTools);
                    $key = key($personalTools);
                    array_shift($personalTools);

                    ?><div id="p-personal"><?php
                        echo $this->makeListItem( $key, $item, array( 'tag' => 'span' ) );
                        if ( count( $personalTools ) > 0 ) {
                            ?>
                            <div class="menu">
                                <ul<?php $this->html( 'userlangattributes' ) ?>>
                                    <?php
                                    foreach ( $personalTools as $key => $item )
                                    { echo $this->makeListItem( $key, $item ); }
                                    ?>
                                </ul>
                            </div>
                            <?php
                        }
                        else {
                            echo 'not available';
                        }
                    ?></div><?php
                }
                break;
            case 'SEARCH':
                ?>
                <div id="p-search">
                    <h5<?php $this->html( 'userlangattributes' ) ?>><label for="searchInput"><?php $this->msg( 'search' ) ?></label></h5>
                    <form action="<?php $this->text( 'wgScript' ) ?>" id="searchform">
                        <input type='hidden' name="title" value="<?php $this->text( 'searchtitle' ) ?>"/>
                        <input type="search" name="q">
                        <input type="submit" value="Search">
                        <!-- <div id="simpleSearch">< ?php -->
                            <!-- // if ( $this->data['rtl'] ) -->
                            <!-- // { echo $this->makeSearchButton( 'image', array( 'id' => 'searchButton', 'src' => $this->getStylePath( 'resources/images/search-rtl.png' ) ) ); } -->
                            <!-- // echo $this->makeSearchInput( array( 'id' => 'searchInput', 'type' => 'text' ) ); -->
                            <!-- // if ( !$this->data['rtl'] ) -->
                            <!-- // { echo $this->makeSearchButton( 'image', array( 'id' => 'searchButton', 'src' => $this->getStylePath( 'resources/images/search-ltr.png' ) ) ); } -->
                        <!-- ? ></div> -->
                    </form>
                </div>
                <?php
                break;
			}
			echo "\n<!-- /{$name} -->\n";
		}
	}

    private function getStylePath( $name ) {
        if ( $this->getSkin()->stylename === null )
        { throw new MWException( "\$stylename must be set to use getStylePath()" ); }
		return $this->getSkin()->getConfig()->get( 'StylePath' ) . "/{$this->getSkin()->stylename}/$name";
    }

    private function renderMainBody() {
        ?>
        <!-- content -->
        <div id="cpp-content-base">
            <div id="content">
                <a id="top"></a>
                <div id="mw-js-message" style="display:none;"<?php $this->html( 'userlangattributes' ) ?>></div>
                <?php if ( $this->data['sitenotice'] ): ?>
                <!-- sitenotice -->
                <div id="siteNotice"><?php $this->html( 'sitenotice' ) ?></div>
                <!-- /sitenotice -->
                <?php endif; ?>
                <!-- firstHeading -->
                <h1 id="firstHeading" class="firstHeading"><?php $this->html( 'title' ) ?></h1>
                <!-- /firstHeading -->
                <!-- bodyContent -->
                <div id="bodyContent">
                    <?php if ( $this->data['isarticle'] ) { ?>
                    <!-- tagline -->
                    <div id="siteSub"><?php $this->msg( 'tagline' ) ?></div>
                    <!-- /tagline -->
                    <?php } ?>
                    <!-- subtitle -->
                    <div id="contentSub"<?php $this->html( 'userlangattributes' ) ?>><?php $this->html( 'subtitle' ) ?></div>
                    <!-- /subtitle -->
                    <?php if ( $this->data['undelete'] ) { ?>
                    <!-- undelete -->
                    <div id="contentSub2"><?php $this->html( 'undelete' ) ?></div>
                    <!-- /undelete -->
                    <?php } ?>
                    <?php if( $this->data['newtalk'] ) { ?>
                    <!-- newtalk -->
                    <div class="usermessage"><?php $this->html( 'newtalk' )  ?></div>
                    <!-- /newtalk -->
                    <?php } ?>
                    <!-- bodycontent -->
                    <?php $this->html( 'bodycontent' ) ?>
                    <!-- /bodycontent -->
                    <?php if ( $this->data['printfooter'] ) { ?>
                    <!-- printfooter -->
                    <div class="printfooter">
                    <?php $this->html( 'printfooter' ); ?>
                    </div>
                    <!-- /printfooter -->
                    <?php } ?>
                    <?php if ( $this->data['catlinks'] ) { ?>
                    <!-- catlinks -->
                    <?php $this->html( 'catlinks' ); ?>
                    <!-- /catlinks -->
                    <?php } ?>
                    <?php if ( $this->data['dataAfterContent'] ) { ?>
                    <!-- dataAfterContent -->
                    <?php $this->html( 'dataAfterContent' ); ?>
                    <!-- /dataAfterContent -->
                    <?php } ?>
                    <div class="visualClear"></div>
                    <!-- debughtml -->
                    <?php $this->html( 'debughtml' ); ?>
                    <!-- /debughtml -->
                </div>
                <!-- /bodyContent -->
            </div>
        </div>
        <!-- /content -->
        <?php
    }

    private function renderMainFooter() {
        ?>
        <!-- footer -->
        <div id="cpp-footer-base" class="noprint">
            <div id="footer"<?php $this->html( 'userlangattributes' ) ?>>
                <?php $this->renderBottomNavigation(); ?>
                <?php $this->renderToolbox(); ?>
                <?php $this->renderSubFooter(); ?>
            </div>
        </div>
        <!-- /footer -->
        <?php
        $this->printTrail();
    }

    private function renderBottomNavigation() {
        $content = $this->data['sidebar']['navigation'];

        $msg = 'navigation';
        $msg_obj = wfMessage( $msg );
        $message = htmlspecialchars($msg_obj->exists() ? $msg_obj->text() : $msg);

        ?>
        <div id="cpp-navigation">
            <h5><?php echo $message; ?></h5>
            <ul>
                <?php
                foreach( $content as $key => $val ) {
                    echo $this->makeListItem( $key, $val );
                }
                ?>
            </ul>
        </div>
        <?php
    }

    private function renderToolbox() {
        $name = 'tb';

        $toolbox = $this->getSkin()->makeToolbox(
			$this->data['nav_urls'],
			$this->data['feeds']
		);

		// Merge content that might be added to the toolbox section by hook
		if ( isset( $this->data['sidebar']['TOOLBOX'] ) )
        { $toolbox = array_merge( $toolbox, $this->data['sidebar']['TOOLBOX'] ?? [] ); }

        $msg = 'toolbox';
        $msg_obj = wfMessage( $msg );
        $message = htmlspecialchars($msg_obj->exists() ? $msg_obj->text() : $msg);

        ?>
        <div id="cpp-toolbox">
            <h5><span><?php echo $message; ?></span><a href="#"></a></h5>
            <ul>
                <?php
                foreach ( $toolbox as $key => $val )
                { echo $this->makeListItem( $key, $val ); }
                // $callback = function ( $old = null, &$ttl = null ) { return []; };
                // wfRunHooks( 'SidebarBeforeOutput', array( &$this, $callback ));
                ?>
            </ul>
        </div>
        <?php
    }

    private function renderSubFooter() {
        if ( $this->data['language_urls'] ) { $this->renderLanguages(); }

        foreach ( $this->getFooterLinks() as $category => $links ) {
        ?><ul id="footer-<?php echo $category ?>"><?php
            foreach( $links as $link ) {
                ?><li id="footer-<?php echo $category ?>-<?php echo $link ?>"><?php $this->html( $link ) ?></li><?php
            }
        ?></ul><?php
        }

        $footericons = $this->get( 'footericons' );
        // Unset any icons which don't have an image
        foreach ( $footericons as $footerIconsKey => &$footerIconsBlock ) {
            foreach ( $footerIconsBlock as $footerIconKey => $footerIcon ) {
                if ( !is_string( $footerIcon ) && !isset( $footerIcon['src'] ) )
                { unset( $footerIconsBlock[$footerIconKey] ); }
            }
            if ( $footerIconsBlock === [] )
            { unset( $footericons[$footerIconsKey] ); }
        }

        if ( count( $footericons ) > 0 ) {
            ?><ul id="footer-icons" class="noprint"><?php
                foreach ( $footericons as $blockName => $footerIcons ) {
                    ?><li id="footer-<?php echo htmlspecialchars( $blockName ); ?>ico"><?php
                        foreach ( $footerIcons as $icon )
                        { echo $this->getSkin()->makeFooterIcon( $icon ); }
                    ?></li><?php
                }
            ?></ul><?php
        }
        else { ?>no icons<?php }
        ?><div style="clear:both"></div><?php
    }

    private function renderLanguages() {
        $content = $this->data['language_urls'];

        $msg = 'otherlanguages';
        $msg_obj = wfMessage( $msg );
        $message = htmlspecialchars($msg_obj->exists() ? $msg_obj->text() : $msg);

        ?>
        <div id="cpp-languages">
            <div><ul><li><?php echo $message; ?></li></ul></div>
            <div><ul>
                <?php
                foreach ( $content as $key => $val )
                { echo $this->makeListItem( $key, $val ); }
                ?>
            </ul></div>
        </div>
        <?php
    }
}
