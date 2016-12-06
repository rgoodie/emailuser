<?php


function _emailusers_form($form, &$form_state)
{




    $form = array();

    // find users
    // Using filters to remove one-by-one. This makes it easy to define a rulesset.

    $users = array_filter(entity_load('user'), function ($item) {
        // skip admin
        if ($item->uid <= 1) {
            return false;
        } else {
            return true;
        }
    });

    // helps me remember struct
    // dpm($users[10]);


    // loop create
    foreach ($users as $u) {
        $form[$u->uid] = array(
            '#type' => 'checkbox',
            '#title' => t($u->realname),
        );
    }


    $form['message'] = array(
        '#type' => 'text_format',
        '#title' => t('Message'),
    );


    $form['actions']['email'] = array(
        '#type' => 'submit',
        '#value' => t('Email'),
        '#submit' => array('_emailusers_submit_send'),
    );

    return $form;


}


function _emailusers_submit_send($form, &$form_state)
{

    // pull out users
    $selected_user_uids = array();
    foreach ($form_state['values'] as $k => $v) {
        if (is_numeric($k) && $v == 1) {
            $selected_user_uids[] = $k;
        }
    }

    dpm($form_state['values']);

    // get basic properties
    $from = variable_get('site_mail', false);
    $subject = t(variable_get('emailusers_default_subj_line', 'Email from this site: ' . variable_get('site_name', '')));
    $body = $form_state['values']['message']['value'];

    // process selected users
    foreach(user_load_multiple($selected_user_uids) as $u) {

        $to = $u->mail;

        simple_mail_queue($from, $to, $subject, $body);
    }




}
